#include "redundantCodeChecker.h"
#include <iostream>
#include <string>
#include <cstring>

// Extract the text a node spans from the raw source.
std::string RedundantCodeChecker::nodeText(TSNode node, const std::string& source) {
    uint32_t start = ts_node_start_byte(node);
    uint32_t end = ts_node_end_byte(node);
    return source.substr(start, end - start);
}

// Given a declarator node (identifier, init_declarator, pointer_declarator, etc.),
std::string RedundantCodeChecker::extractIdentifierFromDeclarator(TSNode node, const std::string& source) {
    std::string type = ts_node_type(node);

    if (type == "identifier") {
        return nodeText(node, source);
    }

    // init_declarator: has a "declarator" field (identifier or pointer_declarator etc.)
    // pointer_declarator / reference_declarator: wraps another declarator
    // array_declarator: wraps another declarator
    TSNode declaratorField = ts_node_child_by_field_name(node, "declarator", strlen("declarator"));
    if (!ts_node_is_null(declaratorField)) {
        return extractIdentifierFromDeclarator(declaratorField, source);
    }

    // search children for an identifier
    uint32_t childCount = ts_node_child_count(node);
    for (uint32_t i = 0; i < childCount; ++i) {
        TSNode child = ts_node_child(node, i);
        std::string childType = ts_node_type(child);
        if (childType == "identifier") {
            return nodeText(child, source);
        }
    }

    return ""; // couldn't resolve — e.g. structured bindings, function pointers
}

// Walk a "declaration" node's children and collect (name, node) pairs.
// Handles `int x;` `int x = 5;`and `int x, y = 2;`.
void RedundantCodeChecker::collectDeclarations(TSNode declarationNode, const std::string& source, std::vector<std::pair<std::string, TSNode>>& declarations) {
    uint32_t childCount = ts_node_child_count(declarationNode);
    for (uint32_t i = 0; i < childCount; ++i) {
        TSNode child = ts_node_child(declarationNode, i);
        std::string childType = ts_node_type(child);

        if (childType == "identifier" || childType == "init_declarator" ||
            childType == "pointer_declarator" || childType == "reference_declarator" ||
            childType == "array_declarator") {
            std::string name = extractIdentifierFromDeclarator(child, source);
            if (!name.empty()) {
                declarations.push_back({name, child});
            }
        }
    }
}

// Recursively count identifier nodes matching `name` within scopeNode's subtree.
int RedundantCodeChecker::countIdentifierOccurrences(TSNode scopeNode, const std::string& source, const std::string& name) {
    int count = 0;
    std::string type = ts_node_type(scopeNode);

    if (type == "identifier" && nodeText(scopeNode, source) == name) {
        count++;
    }

    uint32_t childCount = ts_node_child_count(scopeNode);
    for (uint32_t i = 0; i < childCount; ++i) {
        count += countIdentifierOccurrences(ts_node_child(scopeNode, i), source, name);
    }

    return count;
}

// Detects `x == true`, `x == false`, `x != true`, `x != false` style comparisons,
// which should just be `x` or `!x`.
void RedundantCodeChecker::checkBooleanComparisons(TSNode node, const ParsedSource& parsedSource, int& warningCount) {
    std::string type = ts_node_type(node);
    const std::string& source = parsedSource.source;

    if (type == "binary_expression") {
        TSNode opNode = ts_node_child_by_field_name(node, "operator", strlen("operator"));
        std::string op;
        if (!ts_node_is_null(opNode)) {
            op = nodeText(opNode, source);
        } else if (ts_node_child_count(node) >= 3) {
            op = nodeText(ts_node_child(node, 1), source); // fallback: left, operator, right
        }

        if (op == "==" || op == "!=") {
            TSNode left = ts_node_child_by_field_name(node, "left", strlen("left"));
            TSNode right = ts_node_child_by_field_name(node, "right", strlen("right"));

            auto isBoolLiteral = [](TSNode n) {
                if (ts_node_is_null(n)) return false;
                std::string t = ts_node_type(n);
                return t == "true" || t == "false";
            };

            TSNode boolSide = TSNode{};
            TSNode otherSide = TSNode{};
            bool found = false;

            if (isBoolLiteral(left)) {
                boolSide = left;
                otherSide = right;
                found = true;
            } else if (isBoolLiteral(right)) {
                boolSide = right;
                otherSide = left;
                found = true;
            }

            if (found) {
                bool boolValue = (std::string(ts_node_type(boolSide)) == "true");
                bool negate = (op == "==") ? !boolValue : boolValue;

                std::string otherText = nodeText(otherSide, source);
                std::string suggestion = negate ? ("!" + otherText) : otherText;

                int line = ts_node_start_point(node).row + 1;
                std::string exprText = nodeText(node, source);
                std::cout << "Warning: Redundant boolean comparison. \""
                        << exprText << "\" can be simplified to \""
                        << suggestion << "\". "
                        << "(line " << line << ")\n";
                ++warningCount;
            }
        }
    }

    uint32_t childCount = ts_node_child_count(node);
    for (uint32_t i = 0; i < childCount; ++i) {
        checkBooleanComparisons(ts_node_child(node, i), parsedSource, warningCount);
    }
}

void RedundantCodeChecker::visitNode(TSNode node, const ParsedSource& parsedSource, int& warningCount) {
    std::string type = ts_node_type(node);
    const std::string& source = parsedSource.source;

    if (type == "function_definition") {
        TSNode bodyNode = ts_node_child_by_field_name(node, "body", strlen("body"));
        if (!ts_node_is_null(bodyNode)) {
            // Find all declarations inside this function's body (recursively, so nested blocks count too)
            std::vector<std::pair<std::string, TSNode>> declarations;

            // Simple recursive lambda substitute via helper stack-based walk
            std::vector<TSNode> stack = { bodyNode };
            while (!stack.empty()) {
                TSNode current = stack.back();
                stack.pop_back();

                if (std::string(ts_node_type(current)) == "declaration") {
                    collectDeclarations(current, source, declarations);
                }

                uint32_t cc = ts_node_child_count(current);
                for (uint32_t i = 0; i < cc; ++i) {
                    stack.push_back(ts_node_child(current, i));
                }
            }

            for (auto& [name, declNode] : declarations) {
                int occurrences = countIdentifierOccurrences(bodyNode, source, name);
                // 1 occurrence = only the declaration itself -> unused
                if (occurrences <= 1) {
                    int line = ts_node_start_point(declNode).row + 1;
                    std::cout << "Warning: Redundant dead/unused code. \""
                            << name << "\" is declared but never used. "
                            << "(line " << line << ")\n";
                    ++warningCount;
                }
            }
        }
        return;
    }

    uint32_t childCount = ts_node_child_count(node);
    for (uint32_t i = 0; i < childCount; ++i) {
        visitNode(ts_node_child(node, i), parsedSource, warningCount);
    }
}

int RedundantCodeChecker::analyzeSource(const ParsedSource& parsedSource) {
    int warningCount = 0;

    if (parsedSource.tree == nullptr) {
        return warningCount;
    }

    TSNode rootNode = ts_tree_root_node(parsedSource.tree);
    visitNode(rootNode, parsedSource, warningCount);
    checkBooleanComparisons(rootNode, parsedSource, warningCount);

    return warningCount;
}