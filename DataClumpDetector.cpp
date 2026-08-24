#include "DataClumpDetector.h"
#include "NameAnalyzer.h"
#include <map>
#include <algorithm>
#include <iostream>
#include <set>
#include <utility>

void DataClumpDetector::checkForDataClumps(std::vector<Warning>& warnings) {
    struct CandidateInfo {
        std::unordered_set<std::string> variableNames;
        std::vector<int> lineNumbers;
        int counter = 0;
    };

    if(!variableGroups.empty()) {
        createBitsets();
        std::map<std::vector<std::string>, CandidateInfo> candidateMap;
        std::vector<std::string> sortedVariables;

        for(const auto& variable : variableBitsets) {
            sortedVariables.push_back(variable.first);
        }
        sort(sortedVariables.begin(), sortedVariables.end());

        // Get pairs from frequent pairs
        std::vector<std::pair<std::string, std::string>> frequentPairs;
        for(int i = 0; i < sortedVariables.size(); ++i) {
            for(int j = i + 1; j < sortedVariables.size(); ++j) {
                auto bitsOfPair = variableBitsets[sortedVariables[i]] & variableBitsets[sortedVariables[j]];
                if(bitsOfPair.count() > 2) {
                    frequentPairs.push_back({sortedVariables[i], sortedVariables[j]});
                }
            }
        }


        // Get triples from frequent pairs
        for(int i = 0; i < frequentPairs.size(); ++i) {
            for(int j = 0; j < sortedVariables.size(); ++j) {
                if(sortedVariables[j] != frequentPairs[i].first && sortedVariables[j] != frequentPairs[i].second) {
                    auto bitsOfTriple = variableBitsets[frequentPairs[i].first] & variableBitsets[frequentPairs[i].second] & variableBitsets[sortedVariables[j]];
                    if(bitsOfTriple.count() > 2) {
                        std::vector<std::string> triple = {frequentPairs[i].first, frequentPairs[i].second, sortedVariables[j]};
                        std::sort(triple.begin(), triple.end());

                        if(candidateMap.find(triple) == candidateMap.end()) {
                            for(int k = 0; k < variableGroups.size(); ++k) {
                                if(bitsOfTriple.test(k)) {
                                    candidateMap[triple].lineNumbers.push_back(groupLineNumbers[k]);
                                }
                            }
                            candidateMap[triple].variableNames.insert(triple.begin(), triple.end());
                            candidateMap[triple].counter = bitsOfTriple.count();
                        }
                    }
                }
            }
        }
        
        for (const auto& entry : candidateMap) {
            const CandidateInfo& candidateInfo = entry.second;

            if (candidateInfo.counter > 2) {
                std::string variableList;
                for (const auto& variable : candidateInfo.variableNames) {
                    variableList += variable + ", ";
                }
                variableList = variableList.substr(0, variableList.length() - 2);

                std::string lineList;
                for (const auto& line : candidateInfo.lineNumbers) {
                    lineList += std::to_string(line) + ", ";
                }
                lineList = lineList.substr(0, lineList.length() - 2);

                warnings.push_back({
                    candidateInfo.lineNumbers.front(),
                    "Data Clump",
                    "The following lines share the same set of variables: " + lineList + ". Variables: " + variableList + ". Consider using a struct or class to reduce code duplication."
                });
            }
        }
    }
}

void DataClumpDetector::createBitsets() {
    variableBitsets.clear();

    for (size_t groupIndex = 0; groupIndex < variableGroups.size(); ++groupIndex) {
        const auto& variableGroup = variableGroups[groupIndex];
        for (const auto& variable : variableGroup) {
            variableBitsets[variable].set(groupIndex);
        }
    }
}

void DataClumpDetector::storeClumpInfo(std::vector<std::string>& currentVariables, std::vector<int>& currentLines, std::unordered_set<std::string>& variablesInScope) {
    if(variablesInScope.size() > 1 && !currentLines.empty()) {
        currentVariables.erase(std::unique(currentVariables.begin(), currentVariables.end()), currentVariables.end());
        currentLines.erase(std::unique(currentLines.begin(), currentLines.end()), currentLines.end());
        std::vector<std::string> sortedVariables = currentVariables;
        std::sort(sortedVariables.begin(), sortedVariables.end());

        ClumpInfo& clumpInfo = variableClumps[sortedVariables];
        clumpInfo.lineNumbers.insert(clumpInfo.lineNumbers.end(), currentLines.begin(), currentLines.end());
        clumpInfo.counter++;

        variableGroups.push_back(variablesInScope);
        groupLineNumbers.push_back(currentLines.front());
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
        std::unordered_set<std::string> variablesInScope;

        for(uint32_t i = 0; i < paramCount; ++i) {
            TSNode paramNode = ts_node_child(parameterListNode, i);

            if(strcmp(ts_node_type(paramNode), "parameter_declaration") == 0) {
                TSNode identifierNode = nameAnalyzer.findIdentifierNode(paramNode);
                if(!ts_node_is_null(identifierNode)) {
                    std::string name = nameAnalyzer.extractIdentifierName(parsedSource, identifierNode);
                    variablesInScope.insert(name);
                    paramNames.push_back(name);
                }
            }
        }

        std::vector<int> lineNumbers = {nameAnalyzer.getLineNumber(parsedSource, node)};
        storeClumpInfo(paramNames, lineNumbers, variablesInScope);
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
            std::unordered_set<std::string> variablesInScope;

            for(uint32_t i = 0; i < argCount; ++i) {
                TSNode argNode = ts_node_child(argumentListNode, i);
                if(strcmp(ts_node_type(argNode), "identifier") == 0) {
                    std::string name = nameAnalyzer.extractIdentifierName(parsedSource, argNode);
                    variablesInScope.insert(name);
                    argNames.push_back(name);
                }
            }

            std::vector<int> lineNumbers = {nameAnalyzer.getLineNumber(parsedSource, node)};
            storeClumpInfo(argNames, lineNumbers, variablesInScope);
        }
    }
}

std::unordered_set<std::string> DataClumpDetector::checkBinaryExpression(TSNode node, const ParsedSource& parsedSource, std::vector<Warning>& warnings) {
    if(ts_node_is_null(node)) {
        return {};
    }

    std::unordered_set<std::string> variablesInScope;

    uint32_t childCount = ts_node_child_count(node);
    for(uint32_t i = 0; i < childCount; ++i) {
        TSNode childNode = ts_node_child(node, i);
        if(strcmp(ts_node_type(childNode), "identifier") == 0) {
            std::string name = nameAnalyzer.extractIdentifierName(parsedSource, childNode);
            variablesInScope.insert(name);
        } else if(strcmp(ts_node_type(childNode), "binary_expression") == 0) {
            std::unordered_set<std::string> nestedVariables = checkBinaryExpression(childNode, parsedSource, warnings);
            variablesInScope.insert(nestedVariables.begin(), nestedVariables.end());
        }
    }

    return variablesInScope;
}

void DataClumpDetector::checkInsideFunction(TSNode node, const ParsedSource& parsedSource, std::vector<Warning>& warnings) {
    if(ts_node_is_null(node)) {
        return;
    }

    std::unordered_set<std::string> variablesInScope;
    std::vector<std::string> currentVariables;
    std::vector<int> currentLines;

    uint32_t childCount = ts_node_child_count(node);
    for(uint32_t i = 0; i < childCount; ++i) {
        TSNode childNode = ts_node_child(node, i);

        if(strcmp(ts_node_type(childNode), "declaration") == 0) {
            // TSNode identifierNode = ts_node_child_by_field_name(childNode, "declarator", 10);
            // if(!ts_node_is_null(identifierNode)) {
            //     identifierNode = nameAnalyzer.findIdentifierNode(identifierNode);
            //     if(!ts_node_is_null(identifierNode)) {
            //         std::string name = nameAnalyzer.extractIdentifierName(parsedSource, identifierNode);
            //         currentVariables.push_back(name);
            //         variablesInScope.insert(name);
            //         currentLines.push_back(nameAnalyzer.getLineNumber(parsedSource, identifierNode));
            //     }
            // }
        } else if(strcmp(ts_node_type(childNode), "expression_statement") == 0) {
            TSNode expressionNode = ts_node_child(childNode, 0);
            if(strcmp(ts_node_type(expressionNode), "call_expression") == 0) {
                checkCallExpression(expressionNode, parsedSource, warnings);
            } else if(strcmp(ts_node_type(expressionNode), "binary_expression") == 0) {
                variablesInScope = checkBinaryExpression(expressionNode, parsedSource, warnings);
                currentLines = {nameAnalyzer.getLineNumber(parsedSource, expressionNode)};
            }

            storeClumpInfo(currentVariables, currentLines, variablesInScope);
            
            currentVariables.clear();
            currentLines.clear();
        }
        else {
            storeClumpInfo(currentVariables, currentLines, variablesInScope);
            
            currentVariables.clear();
            currentLines.clear();
            checkInsideFunction(childNode, parsedSource, warnings);
        }
    }

    storeClumpInfo(currentVariables, currentLines, variablesInScope);

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