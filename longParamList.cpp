#include "longParamList.h"

#include <iostream>
#include <cstring>
#include<tree_sitter/api.h>

//Traverses the syntax tree to find function_definition node. When it is found, it extracts the function name and checks the number of parameters. If the number of parameters exceeds kMaxParams, it reports a warning.
void longParamList::visitNode(TSNode node, const ParsedSource& parsedSource, std::vector<Warning>& warnings) {
  if(ts_node_is_null(node)){
    return;
  }

  if(strcmp(ts_node_type(node), "function_definition") == 0){
    TSNode declaratorNode = ts_node_child_by_field_name(node, "declarator", strlen("declarator"));

    if(!ts_node_is_null(declaratorNode)){
        TSNode paramListNode = ts_node_child_by_field_name(declaratorNode, "parameters",strlen("parameters"));

        if(!ts_node_is_null(paramListNode)){
            int paramCount = static_cast<int>(ts_node_named_child_count(paramListNode));

            if(paramCount > kMaxParams){
            std::string functionName = extractFunctionName(node, parsedSource.source);

            if(functionName.empty()){
                functionName = "<unnamed function>";
            }

            int line = static_cast<int>(ts_node_start_point(node).row)+1;
            reportLongParamList(functionName, paramCount, line, warnings);
            }
        }
    }
  }
    uint32_t childCount = ts_node_child_count(node);
    for(uint32_t i = 0; i< childCount; ++i){
        visitNode(ts_node_child(node,i), parsedSource, warnings);
    }

}
//Looks for the first identifier node given a subtree. This is used to extract the function name from a function_definition node.
TSNode longParamList::findIDNode(TSNode node) const{
    if(ts_node_is_null(node)){
        return node;
    }

    if(strcmp(ts_node_type(node), "identifier") == 0){
        return node;
    }

    uint32_t childCOunt = ts_node_child_count(node);
    for(uint32_t i = 0; i< childCOunt; ++i){
        TSNode child = ts_node_child(node,i);
        TSNode result = findIDNode(child);

        if(!ts_node_is_null(result)){
            return result;
        }
    }

    return {};
}

//Extracts the function name from a function_definition node. It looks for the declarator node and then finds the identifier node to extract the function name.
std::string longParamList::extractFunctionName(TSNode functionDefNode, const std::string& source) const{
    TSNode declaratorNode = ts_node_child_by_field_name(functionDefNode, "declarator", strlen("declarator"));
    if(ts_node_is_null(declaratorNode)){
        return "";
    }

    TSNode identifierNode = findIDNode(declaratorNode);
    if(ts_node_is_null(identifierNode)){
        return "";
    }

    uint32_t startByte = ts_node_start_byte(identifierNode);
    uint32_t endByte = ts_node_end_byte(identifierNode);
    return source.substr(startByte, endByte - startByte);
}

//Reports a warning message to the console when a function has too many parameters. It includes the function name, the number of parameters, and the line number where the function is defined.
void longParamList::reportLongParamList(const std::string& functionName, int paramCount, int line, std::vector<Warning>& warnings) const{
    warnings.push_back({
        line,
        "Long Parameter List",
        "Function: " + functionName + " has " + std::to_string(paramCount) + " parameters. Consider reducing the number of parameters to improve code readability.",
    });
}

//Analyzes the source tree and parsed code to find functions with too many parameters. It traverses the syntax tree and counts the number of warnings generated.
std::vector<Warning> longParamList::analyzeSource(const ParsedSource& source){
    if(source.tree == nullptr){
        return {};
    }

    std::vector<Warning> warnings;
    TSNode rootNode = ts_tree_root_node(source.tree);
    visitNode(rootNode, source, warnings);

    return warnings;
}