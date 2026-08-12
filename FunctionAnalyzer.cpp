#include "FunctionAnalyzer.h"
#include "NameAnalyzer.h"
#include <iostream>

void FunctionAnalyzer::checkFunctionName(TSNode functionDeclarator, const ParsedSource& parsedSource, std::vector<Warning>& warnings) {
    TSNode functionIdentifierNode = nameAnalyzer.findIdentifierNode(functionDeclarator);

    if(!ts_node_is_null(functionIdentifierNode)) {
        std::string name = nameAnalyzer.extractIdentifierName(parsedSource, functionIdentifierNode);

        if(nameAnalyzer.isPoorName(name, "function")) {
            int line = nameAnalyzer.getLineNumber(parsedSource, functionIdentifierNode);
            nameAnalyzer.outputErrorMessage(name, line, warnings);
        }
    }
}

void FunctionAnalyzer::checkParameterNames(TSNode functionDeclarator, const ParsedSource& parsedSource, std::vector<Warning>& warnings) {
    TSNode parameterListNode = ts_node_child_by_field_name(functionDeclarator, "parameters", 10);
    if(!ts_node_is_null(parameterListNode)) {
        uint32_t paramCount = ts_node_child_count(parameterListNode);

        for(uint32_t i = 0; i < paramCount; ++i) {
            TSNode paramNode = ts_node_child(parameterListNode, i);

            if(strcmp(ts_node_type(paramNode), "parameter_declaration") == 0) {
                TSNode identifierNode = nameAnalyzer.findIdentifierNode(paramNode);

                if(!ts_node_is_null(identifierNode)) {
                    std::string name = nameAnalyzer.extractIdentifierName(parsedSource, identifierNode);
                    TSNode typeNode = ts_node_child_by_field_name(paramNode, "type", 4);
                    std::string variableType = nameAnalyzer.extractIdentifierName(parsedSource, typeNode);

                    if(nameAnalyzer.isPoorName(name, variableType)) {
                        int line = nameAnalyzer.getLineNumber(parsedSource, identifierNode);
                        nameAnalyzer.outputErrorMessage(name, line, warnings);
                    }
                }
            }
        }
    }
}