#include "DataClumpDetector.h"
#include "NameAnalyzer.h"
#include <algorithm>
#include <iostream>

void DataClumpDetector::checkForDataClumps(std::vector<Warning>& warnings) {
    for (const auto& entry : variableClumps) {
        const std::vector<std::string>& paramNames = entry.first;
        const std::vector<std::string>& functionNames = entry.second;

        if (functionNames.size() > 1) {
            std::string paramList;
            for (const auto& paramName : paramNames) {
                paramList += paramName + ", ";
            }
            paramList = paramList.substr(0, paramList.length() - 2);

            std::string functionList;
            for (const auto& functionName : functionNames) {
                functionList += functionName + ", ";
            }
            functionList = functionList.substr(0, functionList.length() - 2);

            warnings.push_back({
                0,
                "Data Clump",
                "The following functions share the same set of parameters: " + functionList + ". Parameters: " + paramList
            });
        }
    }
}

void DataClumpDetector::checkFunctionParams(TSNode node, const ParsedSource& parsedSource, std::vector<Warning>& warnings) {
    TSNode functionDeclarator = ts_node_child_by_field_name(node, "declarator", 10);

    if(ts_node_is_null(functionDeclarator)) {
        return;
    }

    TSNode parameterListNode = ts_node_child_by_field_name(functionDeclarator, "parameters", 10);

    if(!ts_node_is_null(parameterListNode)) {
        uint32_t paramCount = ts_node_child_count(parameterListNode);
        std::vector<std::string> paramNames;

        for(uint32_t i = 0; i < paramCount; ++i) {
            TSNode paramNode = ts_node_child(parameterListNode, i);

            if(strcmp(ts_node_type(paramNode), "parameter_declaration") == 0) {
                TSNode identifierNode = nameAnalyzer.findIdentifierNode(paramNode);
                if(!ts_node_is_null(identifierNode)) {
                    std::string name = nameAnalyzer.extractIdentifierName(parsedSource, identifierNode);
                    paramNames.push_back(name);
                }
            }
        }

        if(!paramNames.empty()) {
            std::sort(paramNames.begin(), paramNames.end());
            variableClumps[paramNames].push_back(nameAnalyzer.extractIdentifierName(parsedSource, node));
        }
    }
}

void DataClumpDetector::visitNode(TSNode node, const ParsedSource& parsedSource, std::vector<Warning>& warnings) {
    if(ts_node_is_null(node)) {
        return;
    }

    if(strcmp(ts_node_type(node), "function_definition") == 0) {
        checkFunctionParams(node, parsedSource, warnings);
    }

    uint32_t childCount = ts_node_child_count(node);
    for(uint32_t i = 0; i < childCount; ++i) {
        TSNode child = ts_node_child(node, i);
        visitNode(child, parsedSource, warnings);
    }
}

std::vector<Warning> DataClumpDetector::analyzeSource(const ParsedSource& parsedSource) {
    TSNode rootNode = ts_tree_root_node(parsedSource.tree);

    std::vector<Warning> warnings;
    visitNode(rootNode, parsedSource, warnings);
    checkForDataClumps(warnings);
    return warnings;
}