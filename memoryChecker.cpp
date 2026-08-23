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


void memoryChecker::visitNode(TSNode node, const ParsedSource& parsedSource, std::vector<Warning>& warnings) {

    //string to hold node type for comparison
    std::string nodeType = ts_node_type(node);

    // detect allocations
    if (nodeType == "new_expression" || nodeType == "call_expression") {
        
        //determine the type of allocation
        std::string text = getNode(node, parsedSource.source);
        if (nodeType == "new_expression" || text.find("malloc") != std::string::npos || text.find("calloc") != std::string::npos) {//is new operator or a function call with malloc or

            //find name of variable being created
            TSNode parent = ts_node_parent(node);
            std::string varName = "ptr";//default name ptr
            if (!ts_node_is_null(parent) && ts_node_type(parent) == "init_declarator") {//makes sure if the parent node exists and is the declarator
                TSNode nameNode = ts_node_child_by_field_name(parent, "declarator", 10);//get the name
                if (!ts_node_is_null(nameNode)) {
                    varName = getNode(nameNode, parsedSource.source);//set varname to the name
                }
            }
            trackedAllocations[varName] = static_cast<int>(ts_node_start_point(node).row + 1);//add name and location to map (+1 for conversion from 0 index to 1 index)
        }
    }

    // check for deletes UPDATE: added support for free(), fixed errors with variable recognition
    if (nodeType == "delete_expression" || nodeType == "call_expression") {

        std::string text = getNode(node, parsedSource.source);
        std::string VarName ="";
        
        // only proceed if it is a delete statement/function call to free
        if (nodeType == "delete_expression" || text.find("free(") != std::string::npos) {
            //search for matching variable in trackedAllocations
            for (auto it = trackedAllocations.begin(); it != trackedAllocations.end(); ) {
                if (text.find(it->first) != std::string::npos) {
                    it = trackedAllocations.erase(it); //if found, remove the name and its associated location
                } else {
                    ++it;
                }
            }
        }
    }

    //if a return statement is reached and trackedAllocations is not empty, print warning
    if (nodeType == "return_statement") {
        for (const auto& alloc : trackedAllocations) {
            warnings.push_back({
                static_cast<int>(ts_node_start_point(node).row + 1),
                "memory-leak",
                "Unreleased memory allocated at  at line " + 
                    std::to_string(alloc.second) + " does not reach a destructor."
            });
        }
    }
}

//recursive traversal helper function
void memoryChecker::traverse(TSNode node, const ParsedSource& parsedSource, std::vector<Warning>& warnings) {
    std::string type = ts_node_type(node);//gets current node type as string
    
    //if an if statement is detected, normal searching changes to account for the branches
    if (type == "if_statement") {
        auto stateBeforeBranch = this->trackedAllocations;//saves a copy of trackedAllocations before the branch splits

        //use tree sitter to find the nodes for each possible path
        TSNode consequence = ts_node_child_by_field_name(node, "consequence", 11);//consequence is code executed if the if statement is true
        TSNode alternative = ts_node_child_by_field_name(node, "alternative", 11);//alternative is if the if statement is false

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
    trackedAllocations.clear();//make sure that the map is empty at the start
    std::vector<Warning> warnings;

    TSNode rootNode = ts_tree_root_node(parsedSource.tree);
    if (ts_node_is_null(rootNode)) {
        return warnings;
    }

    traverse(rootNode, parsedSource, warnings);
    return warnings;
}