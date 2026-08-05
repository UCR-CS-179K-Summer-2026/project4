#include "poorNameChecker.h"
#include <iostream>
#include <sstream>
#include <string>
#include <regex>
#include <vector>
#include <algorithm>
#include <tree_sitter/api.h>

PoorNameChecker::PoorNameChecker() {
    poorNamesMap = {
        {"temp", {{"currentValue", "temporaryValue"}, "Generic temporary variable name"}},
        {"data", {{"userData", "apiData"}, "Does not describe what kind of data it holds"}},
        {"var", {{"variable", "varName"}, "Does not describe the purpose of the variable"}},
        {"test", {{"testCase", "unitTest"}, "Does not describe what the test is for"}},
        {"x", {{"coordinateX", "xValue"}, "Single-letter variable name"}},
        {"y", {{"coordinateY", "yValue"}, "Single-letter variable name"}},
        {"z", {{"coordinateZ", "zValue"}, "Single-letter variable name"}},
        {"flag", {{"isTrue", "isSet"}, "Does not describe the purpose of the flag"}},
        {"m", {{"rows", "maxValue"}, "Non-descriptive variable name"}},
        {"n", {{"columns", "count"}, "Non-descriptive variable name"}},
    };
}

bool PoorNameChecker::isPoorName(const std::string& name, const std::string& type, const std::string& declaratorType) {
    if(name.length() < 3 && name != "id" && name != "i" && name != "j" && name != "k") {
        return true;
    }

    static std::regex poorNamePattern(R"(\b(temp|data|var|test|x|y|z|flag|m|n)\b)");
    if(std::regex_match(name, poorNamePattern)) {
        return true;
    }
    
    static std::regex boolPrefixPattern(R"(\b(is|has|can|should|was|will)[A-Z].*)");
    if(type == "bool" && !std::regex_match(name, boolPrefixPattern)) {
        return true;
    }

    static std::regex restrictedArrayPattern(R"(\b(arr|array|list|vector|vec|list)\b)");
    if((declaratorType == "array_declarator" || declaratorType == "qualified_identifier") && std::regex_match(name, restrictedArrayPattern)) {
        return true;
    }

    return false;
}
std::vector<std::string> PoorNameChecker::recommendNames(const std::string& name) {
    auto it = poorNamesMap.find(name);
    if (it != poorNamesMap.end()) {
        return it->second.suggestedNames;
    }
    return {};
}

std::string PoorNameChecker::extractIdentifierName(const ParsedSource& parsedSource, TSNode identifierNode) {
    uint32_t startByte = ts_node_start_byte(identifierNode);
    uint32_t endByte = ts_node_end_byte(identifierNode);
    return parsedSource.source.substr(startByte, endByte - startByte);
}

void PoorNameChecker::checkVariableDeclaration(TSNode node, const ParsedSource& parsedSource, int& warningCount) {
    if(ts_node_is_null(node)) {
        return;
    }

    TSNode initDeclarator = ts_node_child_by_field_name(node, "declarator", 10);
    std::cout << "Checking variable declaration, node type: " << ts_node_type(ts_node_child_by_field_name(node, "type", 4)) << std::endl;
    std::string declaratorType = ts_node_type(ts_node_child_by_field_name(node, "type", 4)) == "qualified_identifier" ? "qualified_identifier" : ts_node_type(initDeclarator);
    std::cout << "Declarator type: " << declaratorType << std::endl;
    // std::string declaratorType = ts_node_type(initDeclarator);
    if(!ts_node_is_null(initDeclarator)) {
        // Find the declarator's identifier.
        // Identifier holds the name of the variable, extract it from the source code
        TSNode identifierNode = findIdentifierNode(initDeclarator);

        if(!ts_node_is_null(identifierNode)) {
            std::string name = extractIdentifierName(parsedSource, identifierNode);

            TSNode primitiveTypeNode = ts_node_child_by_field_name(node, "type", 4);
            std::string variableType = extractIdentifierName(parsedSource, primitiveTypeNode);

            if(isPoorName(name, variableType, declaratorType)) {
                int line = std::count(parsedSource.source.begin(), parsedSource.source.begin() + ts_node_start_byte(identifierNode), '\n') + 1;
                outputErrorMessage(name, line);
                ++warningCount;
            }
        }
    }
}

void PoorNameChecker::checkFunctionDefinition(TSNode node, const ParsedSource& parsedSource, int& warningCount) {
    if(ts_node_is_null(node)) {
        return;
    }

    TSNode functionDeclarator = ts_node_child_by_field_name(node, "declarator", 10);
    if(!ts_node_is_null(functionDeclarator)) {
        // Get the parameter list
        TSNode parameterListNode = ts_node_child_by_field_name(functionDeclarator, "parameters", 10);

        if(!ts_node_is_null(parameterListNode)) {
            uint32_t paramCount = ts_node_child_count(parameterListNode);

            for(uint32_t i = 0; i < paramCount; ++i) {
                TSNode paramNode = ts_node_child(parameterListNode, i);

                if(strcmp(ts_node_type(paramNode), "parameter_declaration") == 0) {
                    TSNode identifierNode = findIdentifierNode(paramNode);
                    TSNode declaratorTypeNode = ts_node_child_by_field_name(paramNode, "declarator", 10);
                    std::string declaratorType = ts_node_type(declaratorTypeNode);

                    if(!ts_node_is_null(identifierNode)) {
                        std::string name = extractIdentifierName(parsedSource, identifierNode);

                        TSNode typeNode = ts_node_child_by_field_name(paramNode, "type", 4);
                        std::string variableType = extractIdentifierName(parsedSource, typeNode);

                        if(isPoorName(name, variableType, declaratorType)) {
                            int line = std::count(parsedSource.source.begin(), parsedSource.source.begin() + ts_node_start_byte(identifierNode), '\n') + 1;
                            outputErrorMessage(name, line);
                            ++warningCount;
                        }
                    }
                }
            }
        }
    }
}

void PoorNameChecker::outputErrorMessage(const std::string& name, const int& line) {
    if(poorNamesMap.find(name) != poorNamesMap.end()) {
        const SuggestedNamesInfo& info = poorNamesMap[name];
        std::cout << "Warning: Variable name '" << name << "' is considered poor. Reason: " << info.reason << ". Suggested names: ";
        for (const auto& suggestedName : info.suggestedNames) {
            std::cout << suggestedName << ", ";
        }
    } else if(name.length() < 3 && name != "id") {
        std::cout << "Warning: Variable name '" << name << "' is too short. Consider using a more descriptive name.";

    } else {
        std::cout << "Warning: Variable name '" << name << "' is considered poor. Consider using a more descriptive name.";
    }

    std::cout << " (line " << line << ")\n\n";
}

TSNode PoorNameChecker::findIdentifierNode(TSNode node) {
    if(ts_node_is_null(node)) {
        return node;
    }

    if(strcmp(ts_node_type(node), "identifier") == 0) {
        return node;
    }

    uint32_t childCount = ts_node_child_count(node);
    for(uint32_t i = 0; i < childCount; ++i) {
        TSNode child = ts_node_child(node, i);
        TSNode result = findIdentifierNode(child);

        if(!ts_node_is_null(result)) {
            return result;
        }
    }

    return {};
}

void PoorNameChecker::visitNode(TSNode node, const ParsedSource& parsedSource, int& warningCount) {
    if(ts_node_is_null(node)) {
        return;
    }

    if(strcmp(ts_node_type(node), "declaration") == 0) { // The node is declaring a variable
        checkVariableDeclaration(node, parsedSource, warningCount);
    } else if(strcmp(ts_node_type(node), "function_definition") == 0) { // The node is a function definition
        checkFunctionDefinition(node, parsedSource, warningCount);
    }

    uint32_t childCount = ts_node_child_count(node);
    for(uint32_t i = 0; i < childCount; ++i) {
        TSNode child = ts_node_child(node, i);
        visitNode(child, parsedSource, warningCount);
    }
}

int PoorNameChecker::analyzeSource(const ParsedSource& parsedSource) {
    TSNode rootNode = ts_tree_root_node(parsedSource.tree);

    int warningCount = 0;
    visitNode(rootNode, parsedSource, warningCount);

    return warningCount;
}
