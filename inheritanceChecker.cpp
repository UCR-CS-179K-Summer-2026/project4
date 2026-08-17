#include "inheritanceChecker.h"

#include <sstream>
#include <cstring>
#include <tree_sitter/api.h>

//Walks through parse tree to search for class_specifier. If a class_specifier is found, we have a class and we call checkClass to continue.
void InheritanceChecker::visitNode(TSNode node, const ParsedSource& parsedSource, std::vector<Warning>& warnings){
    if (ts_node_is_null(node)){
        return;
    }

    if(strcmp(ts_node_type(node),"class_specifier") == 0){
        checkClass(node, parsedSource, warnings);
    }

    uint32_t childCOunt = ts_node_child_count(node);

    for(uint32_t i = 0; i<childCOunt; ++i){
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
        
        if(strcmp(ts_node_type(child), type) ==0){
            return child;
        }
    }

    return{};
}

//Given a class node, finds and extracts the parent class if it inherits one, else returns an empty list
std::vector<std::string> InheritanceChecker::extractBaseClasses(TSNode classDefNode, const std::string& source) const{
    std::vector <std::string> baseNames;

    TSNode baseClauseNode = findChildByType(classDefNode, "base_class_clause");

    if(ts_node_is_null(baseClauseNode)){
        return baseNames;
    }

    uint32_t childCOunt = ts_node_child_count(baseClauseNode);

        for(uint32_t i = 0; i<childCOunt; ++i){
        TSNode child = ts_node_child(baseClauseNode, i);
        const char* type = ts_node_type(child);
 
        if(strcmp(type, "type_identifier") == 0 || strcmp(type, "qualified_identifier") == 0){
            baseNames.push_back(nodeText(child, source));
        }
    }


    return baseNames;
}

//Given the classDefNode, scans labels to find whether or not the node is using its inherited parent class functions
void InheritanceChecker::scanForInheritance(TSNode classDefNode, const std::string& source, const std::vector<std::string>& baseNames, std::vector<bool> baseUsed, bool& overrideFound) const{
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
    int line = static_cast<int>(ts_node_start_point(classNode).row) + 1;
 
    std::ostringstream message;

    message << "Class '" << className << "' inherits from ";
    for (size_t i = 0; i < unusedBases.size(); ++i) {
        message << "'" << unusedBases[i] << "'";
        if (i + 1 < unusedBases.size()) message << ", ";
    }
    message << " but shows no using " << (unusedBases.size() > 1 ? "them" : "it")
            << " (no qualified Base::member calls, no explicit base constructor call, "
            << "no override/final specifier). Consider removing the inheritance.";
 
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
