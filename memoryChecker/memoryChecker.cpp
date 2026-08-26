#include "memoryChecker.h"
#include <string>
#include <cstring>


//helper function gets the source code text of a node TODO: possibly use getNode from RedundantCodeChecker
std::string memoryChecker::getNode(TSNode node, const std::string& src) {
    //get node start and end
    uint32_t start = ts_node_start_byte(node);
    uint32_t end = ts_node_end_byte(node);
    //check for out of bounds
    if (start >= src.size() || end > src.size() || start >= end) return "";
    //return the substring at the corresponding location
    return src.substr(start, end - start);
}

//ROUND 2: helper to resolve aliases. Goes through pointerAliases to find what the initial allocation name (the one that would be in trackedAllocations) is
std::string memoryChecker::resolveAlias(const std::string& variable) {

    std::string current = variable;//start with given variable

    
    std::set<std::string> visited;//keep track of visited for preventing infinite loops

    //while an alias for current exists, keep going
    while (pointerAliases.find(current) != pointerAliases.end()) {

        if (visited.count(current) > 0) {//already visited this variable means infinite loop, break
            break;
        }

        visited.insert(current);//mark this variable as visited

        current = pointerAliases[current];//follow alias to next pointer
    }

    return current;
}

//ROUND 2
void memoryChecker::visitNode(TSNode node, const ParsedSource& parsedSource, std::vector<Warning>& warnings) {

    //string to hold node type for comparison
    std::string nodeType = ts_node_type(node);



    // detect allocations
    if (nodeType == "new_expression" || nodeType == "call_expression") {
        
        //determine the type of allocation
        std::string text = getNode(node, parsedSource.source);
        bool isAllocation = false;
        if (nodeType == "new_expression"){
            isAllocation = true;
        }
        if (nodeType == "call_expression") {

            if (text.find("malloc(") != std::string::npos || text.find("calloc(") != std::string::npos) {

                isAllocation = true;
            }
        }
         if (isAllocation) {

            //use helper function to find allocation variable
            std::string varName = getAllocationVariable(node, parsedSource.source);

            //store variable and allocation line
            trackedAllocations[varName] =
                static_cast<int>(ts_node_start_point(node).row + 1);
        }
    }

    
    //detect aliases
    if (nodeType == "init_declarator") {

        TSNode declarator = ts_node_child_by_field_name(node, "declarator", 10);
        TSNode value = ts_node_child_by_field_name(node, "value", 5);

        if (!ts_node_is_null(declarator) && !ts_node_is_null(value)) {

            std::string lhs = getNode(declarator, parsedSource.source);//get name of new pointer
            std::string rhs = getNode(value, parsedSource.source);//get name of what it's being initialized with

            
            size_t firstRealChar = lhs.find_first_not_of(" \t*&");//strip pointer/reference symbols from lhs
            if (firstRealChar != std::string::npos) {
                lhs = lhs.substr(firstRealChar);
            }
            std::string original = resolveAlias(rhs);//resolve rhs in case it's already an alias, to find what the original allocation would be called in trackedAllocations

            if (trackedAllocations.find(original) != trackedAllocations.end()) {//only create alias if original is already an allocation we are tracking
                pointerAliases[lhs] = rhs;//add alias to pointerAliases. do not link to original because resolveAlias can be used to find the root of the chain
            }
        }
    }


    // check for deletes, added support for free(), fixed errors with variable recognition
    if (nodeType == "delete_expression" || nodeType == "call_expression") {

        std::string text = getNode(node, parsedSource.source);
        
        //check if is custom deallocator using infor from round 1
        if (nodeType == "call_expression") {
            for (const auto& pair : deallocatingFunctions) {//go through map of deallocating functions
                std::string funcName = pair.first;//take the name
                if(funcName.empty()) continue;
                if (text.rfind(funcName + "(", 0) == 0) {

                    const std::set<int>& deallocatedParameters = pair.second;//get arg list

                    TSNode argumentList = ts_node_child_by_field_name(node, "arguments", 9);

                    if (!ts_node_is_null(argumentList)) {
                        uint32_t argumentCount = ts_node_named_child_count(argumentList);
                        //check each argument
                        for (uint32_t i = 0; i < argumentCount; ++i) {
                            TSNode argument = ts_node_named_child(argumentList, i);
                            if (ts_node_is_null(argument)) {
                                continue;
                            }
                            //is this parameter deallocated?
                            if (deallocatedParameters.count(static_cast<int>(i)) == 0) {
                                continue;
                            }
                            std::string argumentText = getNode(argument, parsedSource.source);

                            std::string originalPointer = resolveAlias(argumentText);//find original pointer in trackedAllocations

                            //search through trackedAllocations for the argument, delete is found
                            auto allocation = trackedAllocations.find(originalPointer);

                            if (allocation != trackedAllocations.end()) {
                                trackedAllocations.erase(allocation);
                            }
                        }
                    }
                    //already know is custom deallocator, so not a delete or free
                    break;
                }
            }
        }

        //if it is a deletion
        if (nodeType == "delete_expression") {

            //find deleted variable
            TSNode argument = ts_node_named_child(node, 0);

            if (!ts_node_is_null(argument)) {

                std::string argumentText = getNode(argument, parsedSource.source);
                
                std::string originalPointer = resolveAlias(argumentText);//use resolveAlias to find original allocation

                auto allocation = trackedAllocations.find(originalPointer);//find in trackedAllocations

                if (allocation != trackedAllocations.end()) {//delete from trackedAllocations
                    trackedAllocations.erase(allocation);
                }
            }
        }
        
        //if it is a free()
         if (nodeType == "call_expression" && (text.rfind("free(", 0) == 0 || text.rfind("std::free(", 0) == 0)) {
            TSNode argumentList = ts_node_child_by_field_name(node, "arguments", 9);


            if (!ts_node_is_null(argumentList)) {

                uint32_t argumentCount = ts_node_named_child_count(argumentList);
                // free() has 1 argument
                if (argumentCount > 0) {
                    TSNode argument = ts_node_named_child(argumentList, 0);

                    std::string argumentText = getNode(argument, parsedSource.source);

                    std::string originalPointer = resolveAlias(argumentText);//find alias original name

                    auto allocation = trackedAllocations.find(originalPointer);//find in trackedAllocations
                    
                    //if found, delete from trackedAllocations
                    if (allocation != trackedAllocations.end()) {
                        trackedAllocations.erase(allocation);
                    }
                }
            }
        }
    }
    //if a return statement is reached and trackedAllocations is not empty, print warning
    if (nodeType == "return_statement") {
    for (const auto& alloc : trackedAllocations) {

        Warning warning = {
            alloc.second,
            "memory-leak",
            "Unreleased memory allocated at line " +
                std::to_string(alloc.second) +
                " does not reach a destructor."
        };

        // check if no existing warning matches the new one
        bool is_unique = std::none_of(warnings.begin(), warnings.end(), [&](const Warning& existing) {
            return existing.category == warning.category &&
                   existing.message == warning.message;
        });

        //if is new warning, add to warnings vector
        if (is_unique) {
            warnings.push_back(warning);
        }
    }
}
}

//ROUND 2: recursive traversal helper function
void memoryChecker::traverse(TSNode node, const ParsedSource& parsedSource, std::vector<Warning>& warnings) {
    std::string type = ts_node_type(node);//gets current node type as string
    
    //if an if statement is detected, normal searching changes to account for the branches
    if (type == "if_statement") {
        auto stateBeforeBranch = this->trackedAllocations;//saves a copy of trackedAllocations before the branch splits

        //use tree sitter to find the nodes for each possible path
        TSNode consequence = ts_node_child_by_field_name(node, "consequence", 11);//consequence is code executed if the if statement is true
        TSNode alternative = ts_node_child_by_field_name(node, "alternative", 11);//alternative is if the condition is false

        //path taken if the if statement is true
        this->trackedAllocations = stateBeforeBranch; //resets trackedAllocations to branch base
        if (!ts_node_is_null(consequence)) {//if there is code in the if block
            traverse(consequence, parsedSource, warnings);//recursive call to go through inside of if block
        }
        auto stateAfterIfBlock = this->trackedAllocations;//saves state after traversing inside of if block

        // path taken if the if statement is false
        this->trackedAllocations = stateBeforeBranch; //resets trackedAllocations to branch base
        if (!ts_node_is_null(alternative)) {
            traverse(alternative, parsedSource, warnings);//recursive call to go through else path
        }
        auto stateAfterElseBlock = this->trackedAllocations;//saves state after traversing inside of else path

        //save still unfreed allocations after going through the branch
        this->trackedAllocations = stateAfterIfBlock;

        //loop through and adds in any unfreed memory allocation from the else path
        for (const auto& pair : stateAfterElseBlock) {
            this->trackedAllocations[pair.first] = pair.second;
        }
        return;
    }

    visitNode(node, parsedSource, warnings);//if the current node is not an if statement, use base logic from visitNode
    
    uint32_t count = ts_node_child_count(node);//goes through all children of current node
    for (uint32_t i = 0; i < count; ++i) {
        TSNode child = ts_node_child(node, i);
        traverse(child, parsedSource, warnings); //traverse recursively for each child
    }
    
}

std::vector<Warning> memoryChecker::analyzeSource(const ParsedSource& parsedSource) {
    trackedAllocations.clear();
    deallocatingFunctions.clear();
    pointerAliases.clear();//make sure that the map is empty at the start
    std::vector<Warning> warnings;

    TSNode rootNode = ts_tree_root_node(parsedSource.tree);
    if (ts_node_is_null(rootNode)) {
        return warnings;
    }

    //ROUND 1: gather info on deallocating functions
    size_t previousCount;
    //do while loop that catches nested deallocating functions
    do {
        previousCount = deallocatingFunctions.size();

        collectDeallocatingFunctions(rootNode, parsedSource);

    } while (deallocatingFunctions.size() > previousCount);

    //ROUND 2: now knowing which functions are deallocating, go through and catch leaks
    traverse(rootNode, parsedSource, warnings);
    return warnings;
}



// ROUND 1: helper function to scan a function's body to see if it deletes a specific parameter
bool memoryChecker::checkIfBodyDeallocates(TSNode node, const std::string& paramName, const std::string& src, std::set<std::string>& parameterAliases) {
    if (ts_node_is_null(node)) return false;

    std::string type = ts_node_type(node);
    std::string text = getNode(node, src);


    //look for aliasing
    if (type == "init_declarator") {
        TSNode declarator = ts_node_child_by_field_name(node, "declarator", 10);//get variable being declared
        TSNode value = ts_node_child_by_field_name(node, "value", 5);//get value used for variable initialization

        if (!ts_node_is_null(declarator) && !ts_node_is_null(value)) {

            std::string lhs = getNode(declarator, src);//get name of lhs variable
            std::string rhs = getNode(value, src);//get value being assigned to it

            size_t firstRealChar = lhs.find_first_not_of(" \t*&");//remove pointer symbols
            if (firstRealChar != std::string::npos) {
                lhs = lhs.substr(firstRealChar);
            }

            if (parameterAliases.count(rhs) > 0) {//if rhs is a previously known parameter name/alias
                parameterAliases.insert(lhs);//record alias
            }
        }
    }

    //search for delete expressions corresponding to paramName
    if (type == "delete_expression") {

        TSNode argumentNode = ts_node_named_child(node, 0);

        if (!ts_node_is_null(argumentNode)) {

            std::string argumentText = getNode(argumentNode, src);

            if (parameterAliases.count(argumentText) > 0) {//if it's a known parameter name/alias, return true
                return true;
            }
        }
    }
    
    //check for free() calls containing param name
    if (type == "call_expression" && (text.rfind("free(", 0) == 0 || text.rfind("std::free(", 0) == 0)) {

        TSNode argListNode = ts_node_child_by_field_name(node, "arguments", 9);//get arguments from function call

        if (!ts_node_is_null(argListNode)) {

            uint32_t argCount = ts_node_named_child_count(argListNode);


            //check every argument of free()
            for (uint32_t i = 0; i < argCount; ++i) {

                //get current argument
                TSNode argNode = ts_node_named_child(argListNode, i);

                if(ts_node_is_null(argNode)) continue;
                
                std::string argText = getNode(argNode, src);

                if (parameterAliases.count(argText) > 0) {//if it's a known parameter name/alias, return true
                    return true;
                }
            }
        }
    }

    //check if a previously known deallocator is used
    if (type == "call_expression") {

        TSNode functionNode = ts_node_child_by_field_name(node, "function", 8);

        TSNode argumentList = ts_node_child_by_field_name(node, "arguments", 9);

        if (!ts_node_is_null(functionNode) && !ts_node_is_null(argumentList)) {

            std::string calledFunction = getNode(functionNode, src);//get name of called function

            auto it = deallocatingFunctions.find(calledFunction);//check if called function is already in deallocatingFunctions

            if (it != deallocatingFunctions.end()) {

                uint32_t argumentCount = ts_node_named_child_count(argumentList);//get number of args

                //check each argument
                for (uint32_t i = 0; i < argumentCount; ++i) {

                    //do we already know that calledFunction deallocate this argument?
                    if (it->second.count(static_cast<int>(i)) == 0) {
                        continue;
                    }

                    //get this argument
                    TSNode argument = ts_node_named_child(argumentList, i);

                    if (ts_node_is_null(argument)) {
                        continue;
                    }

                    std::string argumentText = getNode(argument, src);//get argument name

                    //is our parameter being passed to it?
                    if (parameterAliases.count(argumentText) > 0) {//is this argument name in our known aliases for the parameter
                        return true;
                    }
                }
            }
        }
    }

    // recursively check all children inside the function
    uint32_t count = ts_node_child_count(node);
    for (uint32_t i = 0; i < count; ++i) {
        if (checkIfBodyDeallocates(ts_node_child(node, i), paramName, src, parameterAliases)) {
            return true;
        }
    }
    return false;
}

// ROUND 1: Scans the entire tree looking for function declarations, adds deallocating functions to deallocatingFUnctions
void memoryChecker::collectDeallocatingFunctions(TSNode node, const ParsedSource& parsedSource) {
    if (ts_node_is_null(node)) return;

    std::string type = ts_node_type(node);

    //if function definition detected
    if (type == "function_definition") {
        //get the function name
        TSNode declNode = ts_node_child_by_field_name(node, "declarator", 10);
        std::string funcName = "";
        
        if (!ts_node_is_null(declNode) && std::string(ts_node_type(declNode)) == "function_declarator") {
            TSNode nameNode = ts_node_child_by_field_name(declNode, "declarator", 10);
            if (!ts_node_is_null(nameNode)) {
                funcName = getNode(nameNode, parsedSource.source);
            }

            //check parameters for pointer inputs
            TSNode paramListNode = ts_node_child_by_field_name(declNode, "parameters", 10);
            TSNode bodyNode = ts_node_child_by_field_name(node, "body", 4);
            //if there are parameterr
            if (!ts_node_is_null(paramListNode)) {
                uint32_t paramCount = ts_node_child_count(paramListNode);
                //loop through parameters
                int currentParamIndex = 0;//track which parameter we're looking at


                for (uint32_t i = 0; i < paramCount; ++i) {

                    TSNode param = ts_node_child(paramListNode, i);

                    if (std::string(ts_node_type(param)) == "parameter_declaration") {
                        TSNode pDecl = ts_node_child_by_field_name(param, "declarator", 10);
                        if (!ts_node_is_null(pDecl)) {
                            std::string paramName = getNode(pDecl, parsedSource.source);
                            //strip asterisks/references to get the raw param name
                            size_t aster = paramName.find_first_not_of(" \t*&");
                            if (aster != std::string::npos) {
                                paramName = paramName.substr(aster);
                            }

                            std::set<std::string> parameterAliases;//for local parameter aliases
                            parameterAliases.insert(paramName);

                            //check if body deallocates this parameter
                            if (!paramName.empty() && checkIfBodyDeallocates(bodyNode, paramName, parsedSource.source, parameterAliases)) {

                                deallocatingFunctions[funcName].insert(currentParamIndex);
                            }
                        }

                        currentParamIndex++;
                    }
                }
            }
        }
    }

    //recursive traversal of the rest of the file
    uint32_t count = ts_node_child_count(node);
    for (uint32_t i = 0; i < count; ++i) {
        collectDeallocatingFunctions(ts_node_child(node, i), parsedSource);
    }
}

//gets the variable name associated with an allocation
std::string memoryChecker::getAllocationVariable(TSNode node, const std::string& src) {

    TSNode current = node;

    //walk up the tree until the declaration is found
    while (!ts_node_is_null(current)) {
        //if current node is declarator
        if (std::string(ts_node_type(current)) == "init_declarator") {

            TSNode declarator = ts_node_child_by_field_name(current, "declarator", 10);//save as declarator

            if (!ts_node_is_null(declarator)) {

                std::string varName = getNode(declarator, src);//get variable name being declared

                //strip pointer/reference characters
                size_t firstRealChar =
                    varName.find_first_not_of(" \t*&");

                if (firstRealChar != std::string::npos) {
                    varName = varName.substr(firstRealChar);
                }

                return varName;
            }
        }

        current = ts_node_parent(current);
    }

    return "ptr";
}