#include "DataClumpDetector.h"
#include "NameAnalyzer.h"
#include <algorithm>
#include <iostream>

void DataClumpDetector::checkForDataClumps(std::vector<Warning>& warnings) {
    for (const auto& entry : variableClumps) {
        const std::vector<std::string>& variableNames = entry.first;
        const ClumpInfo& clumpInfo = entry.second;

        if (clumpInfo.counter > 2) {
            std::string variableList;
            for (const auto& variable : variableNames) {
                variableList += variable + ", ";
            }
            variableList = variableList.substr(0, variableList.length() - 2);

            std::string lineList;
            for (const auto& line : clumpInfo.lineNumbers) {
                lineList += std::to_string(line) + ", ";
            }
            lineList = lineList.substr(0, lineList.length() - 2);

            warnings.push_back({
                clumpInfo.lineNumbers.front(),
                "Data Clump",
                "The following lines share the same set of variables: " + lineList + ". Variables: " + variableList + ". Consider using a struct or class to reduce code duplication."
            });
        }
    }
}

void DataClumpDetector::storeClumpInfo(std::vector<std::string>& currentVariables, std::vector<int>& currentLines) {
    if(currentVariables.size() > 1) {
        currentVariables.erase(std::unique(currentVariables.begin(), currentVariables.end()), currentVariables.end());
        currentLines.erase(std::unique(currentLines.begin(), currentLines.end()), currentLines.end());
        std::vector<std::string> sortedVariables = currentVariables;
        std::sort(sortedVariables.begin(), sortedVariables.end());

        ClumpInfo& clumpInfo = variableClumps[sortedVariables];
        clumpInfo.lineNumbers.insert(clumpInfo.lineNumbers.end(), currentLines.begin(), currentLines.end());
        clumpInfo.counter++;
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

        std::vector<int> lineNumbers = {nameAnalyzer.getLineNumber(parsedSource, node)};
        storeClumpInfo(paramNames, lineNumbers);
    }
}

void DataClumpDetector::checkCallExpression(TSNode node, const ParsedSource& parsedSource, std::vector<Warning>& warnings) {
    if(ts_node_is_null(node)) {
        return;
    }

    if(strcmp(ts_node_type(node), "call_expression") == 0) {
        TSNode argumentListNode = ts_node_child_by_field_name(node, "arguments", 9);
        if(!ts_node_is_null(argumentListNode)) {
            uint32_t argCount = ts_node_child_count(argumentListNode);
            std::vector<std::string> argNames;

            for(uint32_t i = 0; i < argCount; ++i) {
                TSNode argNode = ts_node_child(argumentListNode, i);
                if(strcmp(ts_node_type(argNode), "identifier") == 0) {
                    std::string name = nameAnalyzer.extractIdentifierName(parsedSource, argNode);
                    argNames.push_back(name);
                }
            }

            std::vector<int> lineNumbers = {nameAnalyzer.getLineNumber(parsedSource, node)};
            storeClumpInfo(argNames, lineNumbers);
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
        } else if(strcmp(ts_node_type(childNode), "expression_statement") == 0) {
            TSNode expressionNode = ts_node_child(childNode, 0);
            checkCallExpression(expressionNode, parsedSource, warnings);

            storeClumpInfo(currentVariables, currentLines);
            
            currentVariables.clear();
            currentLines.clear();
        }
        else {
            storeClumpInfo(currentVariables, currentLines);
            
            currentVariables.clear();
            currentLines.clear();
            checkInsideFunction(childNode, parsedSource, warnings);
        }
    }

    storeClumpInfo(currentVariables, currentLines);

    currentVariables.clear();
    currentLines.clear();
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