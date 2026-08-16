#include "redundantCodeChecker.h"
#include <iostream>
#include <string>
#include <cstring>

// ---------- Shared Helpers  ----------

std::string RedundantCodeChecker::nodeText(TSNode node, const std::string& source) {
    uint32_t start = ts_node_start_byte(node);
    uint32_t end = ts_node_end_byte(node);
    return source.substr(start, end - start);
}

// ---------- Check 1: unused/dead variables ----------

std::string RedundantCodeChecker::extractIdentifierFromDeclarator(TSNode node, const std::string& source) {
    std::string type = ts_node_type(node);

    if (type == "identifier") {
        return nodeText(node, source);
    }

    TSNode declaratorField = ts_node_child_by_field_name(node, "declarator", strlen("declarator"));
    if (!ts_node_is_null(declaratorField)) {
        return extractIdentifierFromDeclarator(declaratorField, source);
    }

    uint32_t childCount = ts_node_child_count(node);
    for (uint32_t i = 0; i < childCount; ++i) {
        TSNode child = ts_node_child(node, i);
        std::string childType = ts_node_type(child);
        if (childType == "identifier") {
            return nodeText(child, source);
        }
    }

    return "";
}

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

int RedundantCodeChecker::countIdentifierOccurrences(TSNode node, const std::string& source, const std::string& name) {
    int count = 0;

    if (std::string(ts_node_type(node)) == "identifier" && nodeText(node, source) == name) {
        count++;
    }

    uint32_t childCount = ts_node_child_count(node);
    for (uint32_t i = 0; i < childCount; ++i) {
        TSNode child = ts_node_child(node, i);
        std::string childType = ts_node_type(child);

        if (childType == "compound_statement" && blockRedeclares(child, source, name)) {
            continue;
        }

        count += countIdentifierOccurrences(child, source, name);
    }

    return count;
}

TSNode RedundantCodeChecker::findEnclosingScope(TSNode node) {
    TSNode parent = ts_node_parent(node);
    while (!ts_node_is_null(parent)) {
        if (std::string(ts_node_type(parent)) == "compound_statement") {
            return parent;
        }
        parent = ts_node_parent(parent);
    }
    return TSNode{};
}

bool RedundantCodeChecker::blockRedeclares(TSNode blockNode, const std::string& source, const std::string& name) {
    uint32_t count = ts_node_child_count(blockNode);
    for (uint32_t i = 0; i < count; ++i) {
        TSNode child = ts_node_child(blockNode, i);
        if (std::string(ts_node_type(child)) == "declaration") {
            std::vector<std::pair<std::string, TSNode>> decls;
            collectDeclarations(child, source, decls);
            for (auto& [declName, declNode] : decls) {
                if (declName == name) return true;
            }
        }
    }
    return false;
}

void RedundantCodeChecker::checkUnusedVariables(TSNode functionDefNode, const ParsedSource& parsedSource, std::vector<Warning>& warnings) {
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
        TSNode scope = findEnclosingScope(declNode);
        if (ts_node_is_null(scope)) scope = bodyNode;

        int occurrences = countIdentifierOccurrences(scope, source, name);
        if (occurrences <= 1) {
            int line = ts_node_start_point(declNode).row + 1;
            warnings.push_back({
                line,
                "Unused/dead Variable",
                "The variable \"" + name + "\" is declared but never used."
            });
        }
    }
}

// ---------- Check 2: redundant boolean comparisons ----------

void RedundantCodeChecker::checkBooleanComparison(TSNode node, const ParsedSource& parsedSource, std::vector<Warning>& warnings) {
    const std::string& source = parsedSource.source;

    TSNode opNode = ts_node_child_by_field_name(node, "operator", strlen("operator"));
    std::string currOperator;
    if (!ts_node_is_null(opNode)) {
        currOperator = nodeText(opNode, source);
    } else if (ts_node_child_count(node) >= 3) {
        currOperator = nodeText(ts_node_child(node, 1), source);
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

    warnings.push_back({
        line,
        "Redundant boolean comparison",
        " \"" + exprText + "\" can be simplified to \"" + suggestion + "\"."
    });
}

// ---------- Check 3: redundant if/else returning boolean literals ----------

TSNode RedundantCodeChecker::unwrapToReturnStatement(TSNode node) {
    if (ts_node_is_null(node)) return TSNode{};

    std::string type = ts_node_type(node);

    if (type == "return_statement") {
        return node;
    }

    else if (type == "else_clause") {
        uint32_t count = ts_node_child_count(node);
        if (count >=1) {
            return unwrapToReturnStatement(ts_node_child(node, count -1));
        }
        return TSNode{};
    }

    if (type == "compound_statement") {
        uint32_t count = ts_node_child_count(node);
        TSNode onlyStatement{};
        int statementCount = 0;
        for (uint32_t i = 0; i < count; ++i) {
            TSNode child = ts_node_child(node, i);
            std::string childType = ts_node_type(child);
            if (childType == "{" || childType == "}") continue;
            statementCount++;
            onlyStatement = child;
        }
        if (statementCount == 1){
            return unwrapToReturnStatement(onlyStatement);
        }
        return TSNode{};
    }

    return TSNode{};
}

void RedundantCodeChecker::checkRedundantIfElseReturn(TSNode node, const ParsedSource& parsedSource, std::vector<Warning>& warnings) {
    const std::string& source = parsedSource.source;

    TSNode consequence = ts_node_child_by_field_name(node, "consequence", strlen("consequence"));
    TSNode alternative = ts_node_child_by_field_name(node, "alternative", strlen("alternative"));

    if (ts_node_is_null(consequence) || ts_node_is_null(alternative)) return;

    TSNode thenReturn = unwrapToReturnStatement(consequence);
    TSNode elseReturn = unwrapToReturnStatement(alternative);

    if (ts_node_is_null(thenReturn) || ts_node_is_null(elseReturn)) return;

    auto getBoolReturnValue = [&](TSNode returnStmt, bool& outValue, bool& outFound) {
        outFound = false;
        uint32_t count = ts_node_child_count(returnStmt);
        for (uint32_t i = 0; i < count; ++i) {
            TSNode child = ts_node_child(returnStmt, i);
            std::string childType = ts_node_type(child);
            if (childType == "true" || childType == "false") {
                outValue = (childType == "true");
                outFound = true;
                return;
            }
        }
    };

    bool thenValue, thenFound, elseValue, elseFound;
    getBoolReturnValue(thenReturn, thenValue, thenFound);
    getBoolReturnValue(elseReturn, elseValue, elseFound);

    if (!thenFound || !elseFound || thenValue == elseValue) return;

    TSNode conditionClause = ts_node_child_by_field_name(node, "condition", strlen("condition"));
    TSNode condition = ts_node_is_null(conditionClause) ? TSNode{} : ts_node_child_by_field_name(conditionClause, "value", strlen("value"));
    std::string condText = ts_node_is_null(condition) ? "" : nodeText(condition, source);

    std::string suggestion = thenValue ? condText : ("!" + condText);
    int line = ts_node_start_point(node).row + 1;

    warnings.push_back({
        line,
        "Redundant-If-Else Return Boolean",
        "Can be simplified to \"return " + suggestion + ";\"."
    });
}

// ---------- Single traversal, dispatches by node type ----------

void RedundantCodeChecker::visitNode(TSNode node, const ParsedSource& parsedSource, std::vector<Warning>& warnings) {
    std::string type = ts_node_type(node);

    if (type == "function_definition") {
        checkUnusedVariables(node, parsedSource, warnings);
    }
    else if (type == "binary_expression") {
        checkBooleanComparison(node, parsedSource, warnings);
    }
    else if (type == "if_statement") {
        checkRedundantIfElseReturn(node, parsedSource, warnings);
    }
    else if (type == "compound_statement") {
        checkChainedReturnIfs(node, parsedSource, warnings);
        // checkUnreachableCode(node, parsedSource, warnings);
    }

    uint32_t childCount = ts_node_child_count(node);
    for (uint32_t i = 0; i < childCount; ++i) {
        visitNode(ts_node_child(node, i), parsedSource, warnings);
    }
}

// ---------- Check 4: redundant if/if else/else statements ----------

bool RedundantCodeChecker::alwaysReturns(TSNode statement) {
    if (ts_node_is_null(statement)) return false;
    std::string type = ts_node_type(statement);

    if (type == "return_statement") return true;
    if (type == "compound_statement") return alwaysReturns(getLastStatement(statement));

    return false;
}

void RedundantCodeChecker::checkChainedReturnIfs(TSNode blockNode, const ParsedSource& parsedSource, std::vector<Warning>& warnings) {
    uint32_t count = ts_node_child_count(blockNode);

    int chainLength = 0;
    TSNode chainStart{};

    auto flushChain = [&]() {
        if (chainLength >= 2) {
            int line = ts_node_start_point(chainStart).row + 1;
            warnings.push_back({
                line,
                "Redundant Chained-If Statement",
                std::to_string(chainLength) + " separate if-statements each return unconditionally; "
                "consider an if/else if/else chain instead."
            });
        }
        chainLength = 0;
    };

    for (uint32_t i = 0; i < count; ++i) {
        TSNode child = ts_node_child(blockNode, i);
        std::string type = ts_node_type(child);

        if (type == "comment") continue;
        
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

        flushChain();
    }
    flushChain();
}

TSNode RedundantCodeChecker::getLastStatement(TSNode compoundStatement) {
    uint32_t count = ts_node_child_count(compoundStatement);
    TSNode last{};
    for (uint32_t i = 0; i < count; ++i) {
        TSNode child = ts_node_child(compoundStatement, i);
        std::string childType = ts_node_type(child);
        if (childType == "{" || childType == "}") continue;
        last = child;
    }
    return last;
}

// ---------- Analyze Source ----------

std::vector<Warning> RedundantCodeChecker::analyzeSource(const ParsedSource& parsedSource) {
    std::vector<Warning> warnings;

    if (parsedSource.tree == nullptr) {
        return warnings;
    }

    TSNode rootNode = ts_tree_root_node(parsedSource.tree);
    visitNode(rootNode, parsedSource, warnings);

    return warnings;
}