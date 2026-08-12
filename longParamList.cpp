#include "longParamList.h"

#include <iostream>
#include <cstring>
#include<tree_sitter/api.h>

void longParamList::visitNode(TSNode node, const ParsedSource& parsedSource, int& warningCount){
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
            reportLongParamList(functionName, paramCount, line);
            ++warningCount;
            }
        }
    }
  }
    uint32_t childCount = ts_node_child_count(node);
    for(uint32_t i = 0; i< childCount; ++i){
        visitNode(ts_node_child(node,i), parsedSource, warningCount);
    }

}

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

void longParamList::reportLongParamList(const std::string& functionName, int paramCount, int line ) const{
    std::cout << "Warning: Function: " << functionName << " has " << paramCount << " parameters " 
    << "on line: " << line << ". Consider reducing the number of parameters to improve code readability." << std::endl;
}

int longParamList::analyzeSource(const ParsedSource& source){
    if(source.tree == nullptr){
        return 0;
    }

    int warningCount = 0;
    TSNode rootNode = ts_tree_root_node(source.tree);
    visitNode(rootNode, source, warningCount);

    return warningCount;
}