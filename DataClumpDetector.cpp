#include "DataClumpDetector.h"
#include "NameAnalyzer.h"
#include <map>
#include <algorithm>
#include <iostream>
#include <set>
#include <utility>

void DataClumpDetector::removeSubsetsFromClumps() {
    std::vector<std::vector<std::string>> clumpKeys;

    for (const auto& entry : variableClumps) {
        clumpKeys.push_back(entry.first);
    }

    for (size_t i = 0; i < clumpKeys.size(); ++i) {
        const std::vector<std::string>& subset = clumpKeys[i];

        for (size_t j = 0; j < clumpKeys.size(); ++j) {
            const std::vector<std::string>& superset = clumpKeys[j];

            if (i != j && isSubset(clumpKeys[i], clumpKeys[j])) {
                std::bitset<MAX_GROUPS> subsetBitset = variableBitsets[subset[0]];
                for (size_t k = 1; k < subset.size(); ++k) {
                    subsetBitset &= variableBitsets[subset[k]];
                }

                std::bitset<MAX_GROUPS> supersetBitset = variableBitsets[superset[0]];
                for (size_t k = 1; k < superset.size(); ++k) {
                    supersetBitset &= variableBitsets[superset[k]];
                }

                std::bitset<MAX_GROUPS> intersectionBitset = subsetBitset & ~supersetBitset;
                if(intersectionBitset.count() < 3) {
                    variableClumps.erase(subset);
                    break;
                }
            }
        }
    }
}

bool DataClumpDetector::isSubset(const std::vector<std::string>& subset, const std::vector<std::string>& superset) {
    if(subset.size() >= superset.size()) {
        return false;
    }

    return std::includes(superset.begin(), superset.end(), subset.begin(), subset.end());
}

bool DataClumpDetector::connectsToAll(const std::vector<std::string>& group, const std::string& candidate) {
    std::bitset<MAX_GROUPS> candidateBitset = variableBitsets[candidate];
    for(const auto& member : group) {
        candidateBitset &= variableBitsets[member];
    }

    return candidateBitset.count() > 2;
}

void DataClumpDetector::expandNode(const std::vector<std::string>& variables, std::vector<std::string>& candidates, int index) {
    for(int i = index; i < variables.size(); ++i) {
        const std::string& neighbor = variables[i];
        
        if(connectsToAll(candidates, neighbor)) {
            candidates.push_back(neighbor);

            if(candidates.size() > 2) {
                std::bitset<MAX_GROUPS> combinedBitset = variableBitsets[candidates[0]];
                for(int j = 1; j < candidates.size(); ++j) {
                    combinedBitset &= variableBitsets[candidates[j]];
                }
    
                std::vector<std::string> sortedCandidates = candidates;
                std::sort(sortedCandidates.begin(), sortedCandidates.end());

                ClumpInfo& clumpInfo = variableClumps[sortedCandidates];
                clumpInfo.counter = static_cast<int>(combinedBitset.count());
                clumpInfo.lineNumbers.clear();

                for(int k = 0; k < variableGroups.size(); ++k) {
                    if(combinedBitset.test(k)) {
                        variableClumps[sortedCandidates].lineNumbers.push_back(groupLineNumbers[k]);
                    }
                }
            }

            expandNode(variables, candidates, i + 1);
            candidates.pop_back();
        }
    }
}

void DataClumpDetector::checkForDataClumps(std::vector<Warning>& warnings) {
    if(variableGroups.size() < 2) {
        return;
    }

    // First create the bitsets for each variable, then expand all nodes
    createBitsets();
    std::vector<std::string> variables;
    for(const auto& entry : variableGraphMap) {
        variables.push_back(entry.first);
    }

    std::vector<std::string> candidates;
    expandNode(variables, candidates, 0);

    removeSubsetsFromClumps();
        
    // Generate warnings for each clump found
    for (const auto& entry : variableClumps) {
        const std::vector<std::string>& variables = entry.first;
        const ClumpInfo& clumpInfo = entry.second;

        if (clumpInfo.counter < 3) {
            continue;
        }

        std::string variableList;

        for (const auto& variable : variables) {
            variableList += variable + ", ";
        }

        if (!variableList.empty()) {
            variableList.erase(variableList.length() - 2);
        }

        std::string lineList;
        for (const auto& line : clumpInfo.lineNumbers) {
            lineList += std::to_string(line) + ", ";
        }

        if (!lineList.empty()) {
            lineList.erase(
                lineList.length() - 2);
        }

        std::string generatedName = nameGenerator.generateName(variableList);

        warnings.push_back({
            clumpInfo.lineNumbers.front(),
            "Data Clump",
            "The following lines share the same set of "
            "variables: " + lineList +
            ". Variables: " + variableList +
            ". Consider converting this to a struct or "
            "class with the name: " + generatedName + " "
        });
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

void DataClumpDetector::storeClumpInfo(std::vector<int>& currentLines, std::unordered_set<std::string>& variablesInScope) {
    if(variablesInScope.size() > 1 && !currentLines.empty()) {
        currentLines.erase(std::unique(currentLines.begin(), currentLines.end()), currentLines.end());

        std::vector<int> lineNumbers = currentLines;
        lineNumbers.erase(std::unique(lineNumbers.begin(), lineNumbers.end()), lineNumbers.end());

        variableGroups.push_back(variablesInScope);
        groupLineNumbers.push_back(lineNumbers.front());

        // Update graph
        for (const auto& var1 : variablesInScope) {
            for (const auto& var2 : variablesInScope) {
                if (var1 != var2) {
                    variableGraphMap[var1][var2]++;
                }
            }
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
        storeClumpInfo(lineNumbers, variablesInScope);
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
            storeClumpInfo(lineNumbers, variablesInScope);
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
        } else if(strcmp(ts_node_type(childNode), "expression_statement") == 0) {
            TSNode expressionNode = ts_node_child(childNode, 0);
            if(strcmp(ts_node_type(expressionNode), "call_expression") == 0) {
                checkCallExpression(expressionNode, parsedSource, warnings);
            } else if(strcmp(ts_node_type(expressionNode), "binary_expression") == 0) {
                variablesInScope = checkBinaryExpression(expressionNode, parsedSource, warnings);
                currentLines = {nameAnalyzer.getLineNumber(parsedSource, expressionNode)};
            }

            storeClumpInfo(currentLines, variablesInScope);
            
            currentVariables.clear();
            currentLines.clear();
        }
        else {
            storeClumpInfo(currentLines, variablesInScope);
            
            currentVariables.clear();
            currentLines.clear();
            checkInsideFunction(childNode, parsedSource, warnings);
        }
    }

    storeClumpInfo(currentLines, variablesInScope);

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