#include "longParamList.h"

#include <iostream>
#include <sstream>
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
            std::string refactoredCode = offerRefactoring(node, paramListNode, functionName, parsedSource.source);

            reportLongParamList(functionName, paramCount, line, refactoredCode, warnings);
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
void longParamList::reportLongParamList(const std::string& functionName, int paramCount, int line, const std::string& refactored, std::vector<Warning>& warnings) const{
    std::string message = "Function: " + functionName + " has " + std::to_string(paramCount) + " parameters. Consider reducing the number of parameters to improve code readability." + refactored;

    warnings.push_back({
        line,
        "Long Parameter List",
        message
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

//Outputs the refactored code (puts the paramlist into a struct), alongside original code block
std::string longParamList::offerRefactoring(TSNode functionDefNode, TSNode paramListNode,const std::string& functionName, const std::string& source) const{
      std::string structName = functionName + "Params";

    uint32_t namedCount = ts_node_named_child_count(paramListNode);
    std::vector<std::string> paramNames;

    std::ostringstream structDef;
    structDef << "struct " << structName << " {\n";

    for(uint32_t i = 0; i< namedCount; ++i){
        TSNode paramNode = ts_node_named_child(paramListNode, i);
        structDef << " " << nodeText(paramNode, source) << ";\n";

        TSNode idNode = findIDNode(paramNode);
        if(!ts_node_is_null(idNode)){
            paramNames.push_back(nodeText(idNode, source));
        }
    }

    structDef << "};";

    TSNode bodyNode = ts_node_child_by_field_name(functionDefNode, "body", strlen("body"));
    uint32_t functionStart = ts_node_start_byte(functionDefNode);
    uint32_t headerEnd = ts_node_is_null(bodyNode) ? ts_node_end_byte(functionDefNode) : ts_node_start_byte(bodyNode);

    std::string originalHeader = source.substr(functionStart, headerEnd- functionStart);

    uint32_t paramStart = ts_node_start_byte(paramListNode);
    uint32_t paramEnd = ts_node_end_byte(paramListNode);
    size_t relStart = paramStart - functionStart;
    size_t relLen = paramEnd - paramStart;

    std::string refactoredHeader = originalHeader.substr(0, relStart) + "(" + structName + " params" + originalHeader.substr(relStart + relLen);

    std::ostringstream out;

    out << "\nSuggested refactor: group the long list of parameters into a struct.\n";
    out << "\n--- Original ---\n" << originalHeader << "\n";
    out << "\n--- Suggested struct ---\n" << structDef.str() << "\n";
    out << "\n--- Refactored ---\n" << refactoredHeader << ")" << "\n";
    out << "\nThen update the function body to access each parameter through "
        << "'params.' instead of by name directly";
    
    if (!paramNames.empty()) {
        out << " (e.g. '" << paramNames[0] << "' becomes 'params." << paramNames[0] << "')";
    }
    
    out << ", and update the calls to pass a single " << structName << " argument instead of "
        << namedCount << " separate ones.\n";
 
    return out.str();
}

//Returns the the raw text stored within a passed in node
std::string longParamList::nodeText(TSNode node, const std::string& source) const{
    uint32_t startByte = ts_node_start_byte(node);
    uint32_t endByte = ts_node_end_byte(node);
    
    return source.substr(startByte, endByte-startByte);
}
