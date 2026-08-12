#include "poorNameChecker.h"
#include <iostream>
#include <sstream>
#include <string>
#include <vector>
#include <algorithm>
#include <tree_sitter/api.h>
#include <cstring>

void PoorNameChecker::checkVariableDeclaration(TSNode node, const ParsedSource& parsedSource, std::vector<Warning>& warnings) {
    if(ts_node_is_null(node)) {
        return;
    }

    uint32_t nodeChildCount = ts_node_child_count(node);
    for (uint32_t i = 0; i < nodeChildCount; ++i) {

        TSNode childNode = ts_node_child(node, i);
        const char* childType = ts_node_type(childNode);

        TSNode identifierNode = {};

        if (strcmp(childType, "identifier") == 0) {
            identifierNode = childNode;
        }
        else if (strcmp(childType, "init_declarator") == 0) {
            TSNode declaratorNode = ts_node_child_by_field_name(childNode, "declarator", 10);

            if (!ts_node_is_null(declaratorNode)) {
                identifierNode = nameAnalyzer.findIdentifierNode(declaratorNode);
            }
        }
        else if (strcmp(childType, "array_declarator") == 0) {
            identifierNode = nameAnalyzer.findIdentifierNode(childNode);
        }

        if (!ts_node_is_null(identifierNode)) {
            std::string name = nameAnalyzer.extractIdentifierName(parsedSource, identifierNode);
            TSNode typeNode = ts_node_child_by_field_name(node, "type", 4);
            std::string variableType = nameAnalyzer.extractIdentifierName(parsedSource, typeNode);

            if (nameAnalyzer.isPoorName(name, variableType)) {
                int line = nameAnalyzer.getLineNumber(parsedSource, identifierNode);
                warnings.push_back({
                    line,
                    "poor-name",
                    "Poor identifier name detected: '" + name + "'. Consider using a more descriptive name."
                });
            }
        }
    }
}

void PoorNameChecker::checkFunctionDefinition(TSNode node, const ParsedSource& parsedSource, std::vector<Warning>& warnings) {
    if(ts_node_is_null(node)) {
        return;
    }

    TSNode functionDeclarator = ts_node_child_by_field_name(node, "declarator", 10);
    if(!ts_node_is_null(functionDeclarator)) {
        functionAnalyzer.checkFunctionName(functionDeclarator, parsedSource, warnings);
        functionAnalyzer.checkParameterNames(functionDeclarator, parsedSource, warnings);
    }
}

// void PoorNameChecker::outputErrorMessage(const std::string& name, const int& line) {
//     std::cout << "Warning: Poor identifier name detected: '" << name << "'";
//     std::cout << " (line " << line << "). ";
//     std::cout << "Consider using a more descriptive name.\n\n";
// }

void PoorNameChecker::visitNode(TSNode node, const ParsedSource& parsedSource, std::vector<Warning>& warnings) {
    if(ts_node_is_null(node)) {
        return;
    }

    if(strcmp(ts_node_type(node), "declaration") == 0) {
        checkVariableDeclaration(node, parsedSource, warnings);
    } else if(strcmp(ts_node_type(node), "function_definition") == 0) {
        checkFunctionDefinition(node, parsedSource, warnings);
    }

    uint32_t childCount = ts_node_child_count(node);
    for(uint32_t i = 0; i < childCount; ++i) {
        TSNode child = ts_node_child(node, i);
        visitNode(child, parsedSource, warnings);
    }
}

std::vector<Warning> PoorNameChecker::analyzeSource(const ParsedSource& parsedSource) {
    TSNode rootNode = ts_tree_root_node(parsedSource.tree);

    std::vector<Warning> warnings;
    visitNode(rootNode, parsedSource, warnings);

    return warnings;
}
