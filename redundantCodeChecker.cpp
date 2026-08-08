#include "redundantCodeChecker.h"
#include <iostream>
#include <string>
#include <cstring>

// ---------- Shared Helpers  ----------
// Extract the text a node spans from the raw source.
std::string RedundantCodeChecker::nodeText(TSNode node, const std::string& source) {
    uint32_t start = ts_node_start_byte(node);
    uint32_t end = ts_node_end_byte(node);
    return source.substr(start, end - start);
}

// ---------- Check 1: unused/dead variables ----------

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
// Handles variables in formats: `int x;` `int x = 5;`and `int x, y = 2;`.
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
    if (std::string(ts_node_type(scopeNode)) == "identifier" && nodeText(scopeNode, source) == name) {
        count++;
    }

    uint32_t childCount = ts_node_child_count(scopeNode);
    for (uint32_t i = 0; i < childCount; ++i) {
        count += countIdentifierOccurrences(ts_node_child(scopeNode, i), source, name);
    }

    return count;
}

void RedundantCodeChecker::checkUnusedVariables(TSNode functionDefNode, const ParsedSource& parsedSource, int& warningCount) {
    const std::string& source = parsedSource.source;

    TSNode bodyNode = ts_node_child_by_field_name(functionDefNode, "body", strlen("body"));
    if (ts_node_is_null(bodyNode)) return;

    std::vector<std::pair<std::string, TSNode>> declarations;

    std::vector<TSNode> stack = { bodyNode };
    while (!stack.empty()) {
        TSNode current = stack.back();
        stack.pop_back();

        if (std::string(ts_node_type(current)) == "declaration") {
            collectDeclarations(current, source, declarations);
        }

        uint32_t currentChild = ts_node_child_count(current);
        for (uint32_t i = 0; i < currentChild; ++i) {
            stack.push_back(ts_node_child(current, i));
        }
    }

    for (auto& [name, declNode] : declarations) {
        int occurrences = countIdentifierOccurrences(bodyNode, source, name);
        if (occurrences <= 1) {
            int line = ts_node_start_point(declNode).row + 1;
            std::cout << "Warning: Redundant dead/unused code. The variable \""
                    << name << "\" is declared but never used. "
                    << "(line " << line << ")\n";
            ++warningCount;
        }
    }
}


// ---------- Check 2: redundant boolean comparisons ----------

// Detects `x == true`, `x == false`, `x != true`, `x != false` style comparisons,
// which should just be `x` or `!x`.
void RedundantCodeChecker::checkBooleanComparison(TSNode node, const ParsedSource& parsedSource, int& warningCount) {
    const std::string& source = parsedSource.source;

    TSNode opNode = ts_node_child_by_field_name(node, "operator", strlen("operator"));
    std::string currOperator;
    if (!ts_node_is_null(opNode)) {
        currOperator = nodeText(opNode, source);
    } else if (ts_node_child_count(node) >= 3) {
        currOperator = nodeText(ts_node_child(node, 1), source); // fallback: left, operator, right
    }

    if (currOperator != "==" && currOperator != "!=") return;

    TSNode left = ts_node_child_by_field_name(node, "left", strlen("left"));
    TSNode right = ts_node_child_by_field_name(node, "right", strlen("right"));

    auto isBoolLiteral = [](TSNode n) {
        if (ts_node_is_null(n)) return false;
        std::string currType = ts_node_type(n);
        return currType == "true" || currType == "false";
    };

    TSNode boolSide{}, otherSide{};
    bool found = false;

    if (isBoolLiteral(left)) {
        boolSide = left; otherSide = right; found = true;
    } else if (isBoolLiteral(right)) {
        boolSide = right; otherSide = left; found = true;
    }

    if (!found) return;

    bool boolValue = (std::string(ts_node_type(boolSide)) == "true");
    bool negate = (currOperator == "==") ? !boolValue : boolValue;

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

// ---------- Check 3: redundant if/else returning boolean literals ----------


// ---------- Single traversal, dispatches by node type ----------

// one recursive traversal of the actual AST, 
//dispatching to focused checks by real node type (ex. init_declarator, pointer_declarator, else_clause)
void RedundantCodeChecker::visitNode(TSNode node, const ParsedSource& parsedSource, int& warningCount) {
    std::string type = ts_node_type(node);

    if (type == "function_definition") {
        checkUnusedVariables(node, parsedSource, warningCount);
    } else if (type == "binary_expression") {
        checkBooleanComparison(node, parsedSource, warningCount);
    }
     // } else if (type == "if_statement") {
    //     checkRedundantIfElseReturn(node, parsedSource, warningCount);
    // }
     else if (type == "compound_statement") {
        checkChainedReturnIfs(node, parsedSource, warningCount);
    }

    uint32_t childCount = ts_node_child_count(node);
    for (uint32_t i = 0; i < childCount; ++i) {
        visitNode(ts_node_child(node, i), parsedSource, warningCount);
    }
}

// ---------- Check 4: redundant if/if else/else statements ----------

// Does this statement unconditionally return? Handles `return x;` directly,
// and `{ ... return x; }` where the return is the last statement in the block.
bool RedundantCodeChecker::alwaysReturns(TSNode statement) {
    if (ts_node_is_null(statement)) return false;
    std::string type = ts_node_type(statement);

    if (type == "return_statement") return true;

    if (type == "compound_statement") {
        uint32_t count = ts_node_child_count(statement);
        TSNode lastStatement{};
        for (uint32_t i = 0; i < count; ++i) {
            TSNode child = ts_node_child(statement, i);
            std::string childType = ts_node_type(child);
            if (childType == "{" || childType == "}") continue;
            lastStatement = child;
        }
        return alwaysReturns(lastStatement);
    }

    return false;
}

// Looks at direct children of a block for runs of 2+ sibling if-statements,
// each with no else and an unconditional return, a chain that should be if/else if/else.
void RedundantCodeChecker::checkChainedReturnIfs(TSNode blockNode, const ParsedSource& parsedSource, int& warningCount) {
    const std::string& source = parsedSource.source;
    uint32_t count = ts_node_child_count(blockNode);

    int chainLength = 0;
    TSNode chainStart{};

    auto flushChain = [&]() {
        if (chainLength >= 2) {
            int line = ts_node_start_point(chainStart).row + 1;
            std::cout << "Warning: Redundant conditional statement. "
                    << chainLength << " separate if-statements each return unconditionally; "
                    << "consider an if/else if/else chain instead. "
                    << "(starting line " << line << ")\n";
            ++warningCount;
        }
        chainLength = 0;
    };

    for (uint32_t i = 0; i < count; ++i) {
        TSNode child = ts_node_child(blockNode, i);
        std::string type = ts_node_type(child);

        if (type == "if_statement") {
            TSNode alternative = ts_node_child_by_field_name(child, "alternative", strlen("alternative"));
            TSNode consequence = ts_node_child_by_field_name(child, "consequence", strlen("consequence"));

            bool noElse = ts_node_is_null(alternative);
            bool returns = alwaysReturns(consequence);

            if (noElse && returns) {
                if (chainLength == 0) chainStart = child;
                chainLength++;
                continue;
            }
        }

        // Any non-qualifying statement breaks the chain
        flushChain();
    }
    flushChain(); // catch a chain that runs to the end of the block
}

// ---------- Analyze Source ----------

int RedundantCodeChecker::analyzeSource(const ParsedSource& parsedSource) {
    int warningCount = 0;

    if (parsedSource.tree == nullptr) {
        return warningCount;
    }

    TSNode rootNode = ts_tree_root_node(parsedSource.tree);
    visitNode(rootNode, parsedSource, warningCount);

    return warningCount;
}