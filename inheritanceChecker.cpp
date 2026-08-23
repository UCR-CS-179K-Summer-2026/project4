#include "inheritanceChecker.h"

#include <iostream>
#include <sstream>
#include <cstring>
#include <algorithm>
#include <tree_sitter/api.h>

//Walks through parse tree to search for class_specifier. If a class_specifier is found, we have a class and we call checkClass to continue.
void InheritanceChecker::visitNode(TSNode node, const ParsedSource& parsedSource, std::vector<Warning>& warnings){
    if (ts_node_is_null(node)){
        return;
    }

    if(strcmp(ts_node_type(node),"class_specifier") == 0){
        checkClass(node, parsedSource, warnings);
    }

    uint32_t childCount = ts_node_child_count(node);

    for(uint32_t i = 0; i<childCount; ++i){
        visitNode(ts_node_child(node,i), parsedSource, warnings);
    }
}

//Extracts the raw text stored in a given node
std::string InheritanceChecker::nodeText(TSNode node, const std::string& source) const{
    uint32_t startByte = ts_node_start_byte(node);
    uint32_t endByte = ts_node_end_byte(node);
    return source.substr(startByte, endByte - startByte);
}

//Finds the child of a given node
TSNode InheritanceChecker::findChildByType(TSNode node, const char* type) const{
    uint32_t childCount = ts_node_child_count(node);

    for (uint32_t i = 0; i<childCount; ++i){
        TSNode child = ts_node_child(node,i);

        if(strcmp(ts_node_type(child), type) == 0){
            return child;
        }
    }

    return{};
}

//Given a class node, finds and extracts the parent class if it inherits one, else returns an empty list
std::vector<std::string> InheritanceChecker::extractBaseClasses(TSNode classDefNode, const std::string& source) const{
    std::vector<std::string> baseNames;

    TSNode baseClauseNode = findChildByType(classDefNode, "base_class_clause");

    if(ts_node_is_null(baseClauseNode)){
        return baseNames;
    }

    uint32_t childCount = ts_node_child_count(baseClauseNode);

    for(uint32_t i = 0; i<childCount; ++i){
        TSNode child = ts_node_child(baseClauseNode, i);
        const char* type = ts_node_type(child);

        if(strcmp(type, "type_identifier") == 0 || strcmp(type, "qualified_identifier") == 0){
            baseNames.push_back(nodeText(child, source));
        }
    }

    return baseNames;
}

//Given the classDefNode, scans labels to find whether or not the node is using its inherited parent class functions
void InheritanceChecker::scanForInheritance(TSNode classDefNode, const std::string& source, const std::vector<std::string>& baseNames, std::vector<bool>& baseUsed, bool& overrideFound) const{
    if (ts_node_is_null(classDefNode)) {
        return;
    }

    const char* type = ts_node_type(classDefNode);

    if (strcmp(type, "qualified_identifier") == 0) {
        TSNode scopeNode = ts_node_child_by_field_name(classDefNode, "scope", strlen("scope"));
        if (!ts_node_is_null(scopeNode)) {
            std::string scopeText = nodeText(scopeNode, source);
            for (size_t i = 0; i < baseNames.size(); ++i) {
                if (baseNames[i] == scopeText) {
                    baseUsed[i] = true;
                }
            }
        }
    } else if (strcmp(type, "field_initializer") == 0) {
        if (ts_node_named_child_count(classDefNode) > 0) {
            TSNode targetNode = ts_node_named_child(classDefNode, 0);
            std::string targetText = nodeText(targetNode, source);
            for (size_t i = 0; i < baseNames.size(); ++i) {
                if (baseNames[i] == targetText) {
                    baseUsed[i] = true;
                }
            }
        }
    } else if (strcmp(type, "virtual_specifier") == 0) {
        overrideFound = true;
    }

    uint32_t childCount = ts_node_child_count(classDefNode);
    for (uint32_t i = 0; i < childCount; ++i) {
        scanForInheritance(ts_node_child(classDefNode, i), source, baseNames, baseUsed, overrideFound);
    }
}

//Puts extractBaseClasses and scanForInheritance together to determine whether or not a specific class' inheritance is necessary
void InheritanceChecker::checkClass(TSNode classNode, const ParsedSource& parsedSource, std::vector<Warning>& warnIngs) const{
    std::vector<std::string> baseNames = extractBaseClasses(classNode, parsedSource.source);
    if (baseNames.empty()) {
        return; //noting to scan as no inheritance
    }

    TSNode bodyNode = ts_node_child_by_field_name(classNode, "body", strlen("body"));
    if (ts_node_is_null(bodyNode)) {
        return; //only forward declaration
    }

    std::vector<bool> baseUsed(baseNames.size(), false);
    bool overrideFound = false;
    scanForInheritance(bodyNode, parsedSource.source, baseNames, baseUsed, overrideFound);

    std::vector<std::string> unusedBases;
    if (!overrideFound) {
        for (size_t i = 0; i < baseNames.size(); ++i) {
            if (!baseUsed[i]) {
                unusedBases.push_back(baseNames[i]);
            }
        }
    }

    if (unusedBases.empty()) {
        return;
    }

    TSNode nameNode = ts_node_child_by_field_name(classNode, "name", strlen("name"));
    std::string className = ts_node_is_null(nameNode) ? "<unnamed class>" : nodeText(nameNode, parsedSource.source);

    if(!className.empty() && className != "<unnamed class>") {
        TSNode root = ts_tree_root_node(parsedSource.tree);
        std::vector<std::string> stillUnused;

        for(const auto& baseName : unusedBases){
            bool usedExternally = isBaseUsedExternally(root, parsedSource, className, bodyNode, baseName);
            bool usedViaSlicing = usedExternally ? false : isBaseUsedViaSlicing(root, parsedSource, className, baseName);

            if(!usedExternally && !usedViaSlicing){
                stillUnused.push_back(baseName);
            }
        }

        unusedBases = stillUnused;
    }

    if(unusedBases.empty()){
        return;
    }

    int line = static_cast<int>(ts_node_start_point(classNode).row) + 1;

    std::ostringstream message;

    message << "Class '" << className << "' inherits from ";
    for (size_t i = 0; i < unusedBases.size(); ++i) {
        message << "'" << unusedBases[i] << "'";
        if (i + 1 < unusedBases.size()) message << ", ";
    }
    message << " but shows no evidence of using " << (unusedBases.size() > 1 ? "them" : "it")
            << " (no qualified Base::member calls, no explicit base constructor call, "
            << "no override/final specifier, no outside object access to a base-only member, "
            << "and no assignment or parameter-passing of a subclass object where a base-typed "
            << "value is expected). Consider removing the inheritance.";

    warnIngs.push_back(Warning{line, "unused-inheritance", message.str()});
}

//Grabs the root of the syntax tree and then runs visitNode to start the traversal
std::vector<Warning> InheritanceChecker::analyzeSource(const ParsedSource& parsedSource) {
    std::vector<Warning> warnings;

    if (parsedSource.tree == nullptr) {
        return warnings;
    }

    TSNode rootNode = ts_tree_root_node(parsedSource.tree);
    visitNode(rootNode, parsedSource, warnings);

    return warnings;
}

//Searches the whole tree for a class_specifier whose own name matches `name`.
TSNode InheritanceChecker::findClassByName(TSNode node, const std::string& name, const std::string& source) const{
    if(ts_node_is_null(node)){
        return node;
    }

    if(strcmp(ts_node_type(node), "class_specifier")==0){
        TSNode nameNode = ts_node_child_by_field_name(node, "name", strlen("name"));

        if(!ts_node_is_null(nameNode) && nodeText(nameNode, source) == name){
            return node;
        }
    }

    uint32_t childCount = ts_node_child_count(node);
    for (uint32_t i = 0; i < childCount; ++i){
        TSNode result = findClassByName(ts_node_child(node, i), name, source);
        if(!ts_node_is_null(result)){
            return result;
        }
    }
    return{};
}

//Checks if a base class is used outside the scope of the subclass
bool InheritanceChecker::isBaseUsedExternally(TSNode root, const ParsedSource& parsedSource, const std::string& derivedClassName, TSNode derivedBodyNode, const std::string& baseName) const{
    TSNode baseClassNode = findClassByName(root, baseName, parsedSource.source);

    if(ts_node_is_null(baseClassNode)){
        return false;
    }

    TSNode baseBodyNode = ts_node_child_by_field_name(baseClassNode, "body", strlen("body"));

    if(ts_node_is_null(baseBodyNode)){
        return false;
    }

    std::vector<std::string> baseMemberNames = collectMemberNames(baseBodyNode, parsedSource.source);
    std::vector<std::string> derivedMemberNames = collectMemberNames(derivedBodyNode, parsedSource.source);

    std::vector<std::string> baseOnlyMemberNames;

    for(const auto& name : baseMemberNames){
        if (std::find(derivedMemberNames.begin(), derivedMemberNames.end(), name) == derivedMemberNames.end()){
            baseOnlyMemberNames.push_back(name);
        }
    }

    if(baseOnlyMemberNames.empty()){
        return false;
    }
    std::vector<std::string> instanceNames;
    collectInstanceOfType(root, parsedSource.source, derivedClassName, instanceNames);

    if(instanceNames.empty()){
        return false;
    }

    bool found = false;
    checkExternalMemberUsage(root, parsedSource.source, instanceNames, baseOnlyMemberNames, found);

    return found;
}

//Build a list of variable declarations whose type matches the subclass name
void InheritanceChecker::collectInstanceOfType(TSNode node, const std::string& source, const std::string& typeName, std::vector<std::string>& instanceNames) const{
    if(ts_node_is_null(node)){
        return;
    }

    if (strcmp(ts_node_type(node),"declaration") == 0){
        TSNode typeNode = ts_node_child_by_field_name(node, "type", strlen("type"));

        if(!ts_node_is_null(typeNode) && nodeText(typeNode, source) == typeName){
            TSNode declaratorNode = ts_node_child_by_field_name(node, "declarator", strlen("declarator"));

            if(!ts_node_is_null(declaratorNode)){
                TSNode identifierNode = declaratorNode;
                if(strcmp(ts_node_type(declaratorNode), "identifier") != 0){
                    TSNode inner = ts_node_child_by_field_name(declaratorNode, "declarator", strlen("declarator"));

                    if(!ts_node_is_null(inner)){
                        identifierNode = inner;
                    }
                }

                if(strcmp(ts_node_type(identifierNode), "identifier") == 0){
                    instanceNames.push_back(nodeText(identifierNode, source));
                }

            }
        }
    }

    uint32_t childCount = ts_node_child_count(node);

    for(uint32_t i = 0; i < childCount; ++i){
        collectInstanceOfType(ts_node_child(node,i), source, typeName, instanceNames);
    }

}

//Collects names of all the members of a class body, uses the text from field_identifier node
std::vector<std::string> InheritanceChecker::collectMemberNames(TSNode classBodyNode, const std::string& source) const{
    std::vector<std::string> names;

    if(ts_node_is_null(classBodyNode)){
        return names;
    }

    if(strcmp(ts_node_type(classBodyNode), "field_identifier") == 0){
        names.push_back(nodeText(classBodyNode, source));
    }

    uint32_t childCount = ts_node_child_count(classBodyNode);

    for(uint32_t i =0; i<childCount; ++i){
        std::vector<std::string> childNames = collectMemberNames(ts_node_child(classBodyNode, i),source);
        names.insert(names.end(), childNames.begin(), childNames.end());
    }

    return names;
}

//Walks the entire file looking for variable.member where variable matches an instanceName and member is a memberName
void InheritanceChecker::checkExternalMemberUsage(TSNode node, const std::string& source, const std::vector<std::string>& instanceNames, const std::vector<std::string>& memberNames, bool& found) const{
    if(ts_node_is_null(node) || found ){
        return;
    }

    if(strcmp(ts_node_type(node), "field_expression") == 0){
        TSNode argumentNode = ts_node_child_by_field_name(node, "argument",strlen("argument"));
        TSNode fieldNode = ts_node_child_by_field_name(node, "field", strlen("field"));

        if(!ts_node_is_null(argumentNode) && !ts_node_is_null(fieldNode) && strcmp(ts_node_type(argumentNode), "identifier") == 0){
            std::string argumentText = nodeText(argumentNode, source);
            std::string fieldText = nodeText(fieldNode, source);

            bool isInstance = std::find(instanceNames.begin(), instanceNames.end(), argumentText) != instanceNames.end();
            bool isBaseMember  = std::find(memberNames.begin(), memberNames.end(), fieldText) != memberNames.end();

            if(isInstance && isBaseMember){
                found = true;
                return;
            }
        }
    }

    uint32_t childCount = ts_node_child_count(node);
    for(uint32_t i = 0; i <childCount; ++i){
        checkExternalMemberUsage(ts_node_child(node,i), source, instanceNames, memberNames, found);

        if(found){
            return;
        }
    }

}

//Uses the tree to search for function_definition matching the passed in variable "name"
TSNode InheritanceChecker::findFunctionByName(TSNode node, const std::string& name, const std::string& source) const {
    if (ts_node_is_null(node)) {
        return node;
    }

    if (strcmp(ts_node_type(node), "function_definition") == 0) {
        TSNode declaratorNode = ts_node_child_by_field_name(node, "declarator", strlen("declarator"));
        if (!ts_node_is_null(declaratorNode)) {
            TSNode identifierNode = ts_node_child_by_field_name(declaratorNode, "declarator", strlen("declarator"));
            if (!ts_node_is_null(identifierNode) && strcmp(ts_node_type(identifierNode), "identifier") == 0
                && nodeText(identifierNode, source) == name) {
                return node;
            }
        }
    }

    uint32_t childCount = ts_node_child_count(node);
    for (uint32_t i = 0; i < childCount; ++i) {
        TSNode result = findFunctionByName(ts_node_child(node, i), name, source);
        if (!ts_node_is_null(result)) {
            return result;
        }
    }

    return {};
}


//Walks the file and looks for either assignability using operator or passing into a function a subclass object via function parameter
void InheritanceChecker::scanForSlicingUsage(TSNode node, TSNode root, const std::string& source,const std::string& baseName, const std::vector<std::string>& derivedInstanceNames, const std::vector<std::string>& baseInstanceNames, bool& found) const {
    if (ts_node_is_null(node) || found) {
        return;
    }

    const char* type = ts_node_type(node);

    if (strcmp(type, "call_expression") == 0) {
        TSNode functionNode = ts_node_child_by_field_name(node, "function", strlen("function"));
        TSNode argsNode = ts_node_child_by_field_name(node, "arguments", strlen("arguments"));

        if (!ts_node_is_null(functionNode) && strcmp(ts_node_type(functionNode), "identifier") == 0
            && !ts_node_is_null(argsNode)) {
            std::string calleeName = nodeText(functionNode, source);
            TSNode calleeDef = findFunctionByName(root, calleeName, source);

            if (!ts_node_is_null(calleeDef)) {
                TSNode calleeDeclarator = ts_node_child_by_field_name(calleeDef, "declarator", strlen("declarator"));
                TSNode paramListNode = ts_node_is_null(calleeDeclarator) ? TSNode{} :
                    ts_node_child_by_field_name(calleeDeclarator, "parameters", strlen("parameters"));

                if (!ts_node_is_null(paramListNode)) {
                    uint32_t paramCount = ts_node_named_child_count(paramListNode);
                    uint32_t argCount = ts_node_named_child_count(argsNode);
                    uint32_t n = std::min(paramCount, argCount);

                    for (uint32_t i = 0; i < n; ++i) {
                        TSNode paramNode = ts_node_named_child(paramListNode, i);
                        TSNode paramTypeNode = ts_node_child_by_field_name(paramNode, "type", strlen("type"));

                        if(!ts_node_is_null(paramTypeNode) && nodeText(paramTypeNode, source) == baseName) {
                            TSNode argNode = ts_node_named_child(argsNode, i);
                            
                            if(strcmp(ts_node_type(argNode), "identifier") == 0) {
                                std::string argText = nodeText(argNode, source);
                                
                                if(std::find(derivedInstanceNames.begin(), derivedInstanceNames.end(), argText) != derivedInstanceNames.end()) {
                                    found = true;
                                    return;
                                }
                            }
                        }
                    }
                }
            }
        }
    } else if (strcmp(type, "declaration") == 0) {
        TSNode typeNode = ts_node_child_by_field_name(node, "type", strlen("type"));
        if (!ts_node_is_null(typeNode) && nodeText(typeNode, source) == baseName) {
            TSNode declaratorNode = ts_node_child_by_field_name(node, "declarator", strlen("declarator"));
            
            if (!ts_node_is_null(declaratorNode) && strcmp(ts_node_type(declaratorNode), "init_declarator") == 0) {
                TSNode valueNode = ts_node_child_by_field_name(declaratorNode, "value", strlen("value"));
                
                if (!ts_node_is_null(valueNode) && strcmp(ts_node_type(valueNode), "identifier") == 0) {
                    std::string valueText = nodeText(valueNode, source);
                    
                    if (std::find(derivedInstanceNames.begin(), derivedInstanceNames.end(), valueText) != derivedInstanceNames.end()) {
                        found = true;
                        return;
                    }
                }
            }
        }
    } 
    
    else if (strcmp(type, "assignment_expression") == 0) {
        TSNode leftNode = ts_node_child_by_field_name(node, "left", strlen("left"));
        TSNode rightNode = ts_node_child_by_field_name(node, "right", strlen("right"));

        if (!ts_node_is_null(leftNode) && !ts_node_is_null(rightNode)
            && strcmp(ts_node_type(leftNode), "identifier") == 0
            && strcmp(ts_node_type(rightNode), "identifier") == 0) {
            std::string leftText = nodeText(leftNode, source);
            std::string rightText = nodeText(rightNode, source);

            bool leftIsBaseInstance = std::find(baseInstanceNames.begin(), baseInstanceNames.end(), leftText) != baseInstanceNames.end();
            bool rightIsDerivedInstance = std::find(derivedInstanceNames.begin(), derivedInstanceNames.end(), rightText) != derivedInstanceNames.end();

            if (leftIsBaseInstance && rightIsDerivedInstance) {
                found = true;
                return;
            }
        }
    }

    uint32_t childCount = ts_node_child_count(node);
    for (uint32_t i = 0; i < childCount; ++i) {
        scanForSlicingUsage(ts_node_child(node, i), root, source, baseName, derivedInstanceNames, baseInstanceNames, found);
        if (found) return;
    }
}

//Checks if an object of the sub class is assigned using base class member
bool InheritanceChecker::isBaseUsedViaSlicing(TSNode root, const ParsedSource& parsedSource, const std::string& derivedClassName, const std::string& baseName) const {
    std::vector<std::string> derivedInstanceNames;
    collectInstanceOfType(root, parsedSource.source, derivedClassName, derivedInstanceNames);

    if (derivedInstanceNames.empty()) {
        return false; // no objects of the derived class exist anywhere to pass/assign
    }

    std::vector<std::string> baseInstanceNames;
    collectInstanceOfType(root, parsedSource.source, baseName, baseInstanceNames);

    bool found = false;
    scanForSlicingUsage(root, root, parsedSource.source, baseName, derivedInstanceNames, baseInstanceNames, found);
    return found;
}


void InheritanceChecker::offerRefactoring(TSNode classNode, TSNode baseClauseNode, const std::string& className,const std::vector<std::string>& unusedBases, const std::string& source) const{
    uint32_t classStart = ts_node_start_byte(classNode);
    uint32_t classEnd = ts_node_end_byte(classNode);
    std::string originalText = source.subsyr(classStart, classEnd-classStart);

    uint32_t clauseStart = ts_node_start_byte(baseClauseNode);
    uint32_t clauseEnd = ts_node_end_byte(baseClauseNode);
    size_t relStart = clauseStart - classStart;
    size_t relLen = clauseEnd - clauseStart;
 
    std::string refactoredText = originalText.substr(0, relStart) + originalText.substr(relStart + relLen);

    size_t firstNewline = refactoredText.find('\n');
    std::string headerLine = (firstNewline == std::string::npos) ? refactoredText : refactoredText.substr(0, firstNewline);
    std::string restOfBody = (firstNewline == std::string::npos) ? "" : refactoredText.substr(firstNewline);
 
    std::string cleanedHeader;
    bool lastWasSpace = false;
    for (char c : headerLine) {
        bool isSpace = (c == ' ' || c == '\t');

        if (isSpace && lastWasSpace){
            continue;
        }

        cleanedHeader += c;
        lastWasSpace = isSpace;
    }
 
    std::string cleaned = cleanedHeader + restOfBody;
 
    std::cout << "\nClass '" << className << "' inherits from ";
    for(size_t i = 0;i< unusedBases.size();++i) {
        std::cout << "'" << unusedBases[i] << "'";

        if(i + 1 < unusedBases.size()){
            std::cout << ", ";
        }
    }

    std::cout << " but never uses " << (unusedBases.size()> 1 ? "them" : "it") << ".\n";
    std::cout<< "Would you like to refactor this now and remove the unused inheritance? (y/n): ";
 
    std::string response;
    std::getline(std::cin, response);
 
    if(!response.empty() && (response[0] == 'y' || response[0] == 'Y')) {
        std::cout << "\n--- Original ---\n" << originalText << "\n";
        std::cout << "\n--- Refactored ---\n" << cleaned << ";\n";
        std::cout << "\nChanges made:\n";

        for(const auto& base : unusedBases){
            std::cout << "  - Removed inheritance from '" << base << "'\n";
        }

        std::cout << std::endl;
    } 
    
    else{
        std::cout << "Skipped refactoring for '" << className << "'.\n" << std::endl;
    }

}
