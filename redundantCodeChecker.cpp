#include "redundantCodeChecker.h"
#include <iostream>
#include <string>
#include <cstring>

// ---------- Shared Helpers  ----------

// extracts the exact source text a given TSNode spans, 
//using its start/end byte offsets against the raw source string. 
std::string RedundantCodeChecker::nodeText(TSNode node, const std::string& source) {
    uint32_t start = ts_node_start_byte(node);
    uint32_t end = ts_node_end_byte(node);
    return source.substr(start, end - start);
}

// ---------- Check 1: unused/dead variables ----------

// Given a declarator node
// recursively digs down to find and return the variable name
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

// Handles variables in formats: `int x;` `int x = 5;`and `int x, y = 2;`
// Given a declaration node, walks its direct children and pulls out every declared variable name 
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

// Recursively counts how many identifier nodes matching a given name appear anywhere within a subtree
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

// Given a function_definition node, finds its body, collects every variable declared anywhere
// counts how many times each declared name is referenced
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
// Given a binary_expression node, checks whether its operator is ==/!= 
// and one side is a true/false literal
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

// Recursively unwraps a branch node down to the return_statement inside it
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

// Given an if_statement node, checks whether both its consequence and alternative branches 
// unconditionally return opposite boolean literals (true/false);
void RedundantCodeChecker::checkRedundantIfElseReturn(TSNode node, const ParsedSource& parsedSource, int& warningCount) {
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

    std::cout << "Warning: Redundant if/else returning boolean literals. "
            << "Can be simplified to \"return " << suggestion << ";\". "
            << "(line " << line << ")\n";
    ++warningCount;
}


// ---------- Single traversal, dispatches by node type ----------

// one recursive traversal of the actual AST, dispatching to focused checks by real node type
void RedundantCodeChecker::visitNode(TSNode node, const ParsedSource& parsedSource, int& warningCount) {
    std::string type = ts_node_type(node);

    if (type == "function_definition") {
        checkUnusedVariables(node, parsedSource, warningCount);
    } else if (type == "binary_expression") {
        checkBooleanComparison(node, parsedSource, warningCount);
    }
    else if (type == "if_statement") {
        checkRedundantIfElseReturn(node, parsedSource, warningCount);
    }
     else if (type == "compound_statement") {
        checkChainedReturnIfs(node, parsedSource, warningCount);
    }

    uint32_t childCount = ts_node_child_count(node);
    for (uint32_t i = 0; i < childCount; ++i) {
        visitNode(ts_node_child(node, i), parsedSource, warningCount);
    }
}

// ---------- Check 4: redundant if/if else/else statements ----------

// Checks whether a given statement unconditionally returns, either directly or 
// as the last statement inside a {} block.
bool RedundantCodeChecker::alwaysReturns(TSNode statement) {
    if (ts_node_is_null(statement)) return false;
    std::string type = ts_node_type(statement);

    if (type == "return_statement") { return true; }

    else if (type == "compound_statement") {
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

// Given a compound_statement node, scans its direct children for runs of 2+ consecutive 
//sibling if_statements, each with no else and an unconditional return
void RedundantCodeChecker::checkChainedReturnIfs(TSNode blockNode, const ParsedSource& parsedSource, int& warningCount) {
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

// Gets the root node from the ParsedSource's TSTree, kicks off visitNode()
// returns the total warning count
int RedundantCodeChecker::analyzeSource(const ParsedSource& parsedSource) {
    int warningCount = 0;

    if (parsedSource.tree == nullptr) {
        return warningCount;
    }

    TSNode rootNode = ts_tree_root_node(parsedSource.tree);
    visitNode(rootNode, parsedSource, warningCount);

    return warningCount;
}