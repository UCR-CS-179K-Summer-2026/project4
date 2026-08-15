#include "DataClumpDetector.h"
#include "NameAnalyzer.h"
#include <algorithm>
#include <iostream>

void DataClumpDetector::checkForDataClumps(std::vector<Warning>& warnings) {
    for (const auto& entry : variableClumps) {
        const std::vector<std::string>& variableNames = entry.first;
        const std::vector<int>& lineNumbers = entry.second;

        if (lineNumbers.size() > 1) {
            std::string variableList;
            for (const auto& variable : variableNames) {
                variableList += variable + ", ";
            }
            variableList = variableList.substr(0, variableList.length() - 2);

            std::string lineList;
            for (const auto& line : lineNumbers) {
                lineList += std::to_string(line) + ", ";
            }
            lineList = lineList.substr(0, lineList.length() - 2);

            warnings.push_back({
                lineNumbers.front(),
                "Data Clump",
                "The following lines share the same set of variables: " + lineList + ". Variables: " + variableList + ". Consider using a struct or class to reduce code duplication."
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
            variableClumps[paramNames].push_back(nameAnalyzer.getLineNumber(parsedSource, functionDeclarator));
        }
    }
}

void DataClumpDetector::checkInsideFunction(TSNode node, const ParsedSource& parsedSource, std::vector<Warning>& warnings) {
    if(ts_node_is_null(node)) {
        return;
    }

    std::vector<std::string> currentVariables;
    std::vector<int> currentLines;

    uint32_t childCount = ts_node_child_count(node);
    for(uint32_t i = 0; i < childCount; ++i) {
        TSNode childNode = ts_node_child(node, i);

        if(strcmp(ts_node_type(childNode), "declaration") == 0) {
            TSNode identifierNode = ts_node_child_by_field_name(childNode, "declarator", 10);
            if(!ts_node_is_null(identifierNode)) {
                identifierNode = nameAnalyzer.findIdentifierNode(identifierNode);
                if(!ts_node_is_null(identifierNode)) {
                    std::string name = nameAnalyzer.extractIdentifierName(parsedSource, identifierNode);
                    currentVariables.push_back(name);
                    currentLines.push_back(nameAnalyzer.getLineNumber(parsedSource, identifierNode));
                }
            }
        } else {
            checkInsideFunction(childNode, parsedSource, warnings);
        }
    }

    if(currentVariables.size() > 1) {
        std::sort(currentVariables.begin(), currentVariables.end());
        variableClumps[currentVariables].insert(variableClumps[currentVariables].end(), currentLines.begin(), currentLines.end());
    }
}

void DataClumpDetector::visitNode(TSNode node, const ParsedSource& parsedSource, std::vector<Warning>& warnings) {
    if(ts_node_is_null(node)) {
        return;
    }

    if(strcmp(ts_node_type(node), "function_definition") == 0) {
        checkFunctionParams(node, parsedSource, warnings);
    } else if(strcmp(ts_node_type(node), "compound_statement") == 0) {
        checkInsideFunction(node, parsedSource, warnings);
        return;
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