#include "deadCodeChecker.h"
#include <iostream>
#include <string>
#include <cstring>

// ---------- Check 5: dead/unused code blocks  ----------

TSNode DeadCodeChecker::getLastStatement(TSNode compoundStatement) {
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

bool DeadCodeChecker::alwaysExits(TSNode statement) {
    if (ts_node_is_null(statement)) return false;
    std::string type = ts_node_type(statement);

    if (type == "return_statement" || type == "break_statement" ||
        type == "continue_statement" || type == "goto_statement") {
        return true;
    }
    if (type == "compound_statement") {
        return alwaysExits(getLastStatement(statement));
    }
    if (type == "else_clause") {
        uint32_t count = ts_node_child_count(statement);
        if (count == 0) return false;
        return alwaysExits(ts_node_child(statement, count - 1));
    }
    if (type == "if_statement") {
        TSNode consequence = ts_node_child_by_field_name(statement, "consequence", strlen("consequence"));
        TSNode alternative = ts_node_child_by_field_name(statement, "alternative", strlen("alternative"));
        if (ts_node_is_null(alternative)) return false;          // <-- must short-circuit here
        return alwaysExits(consequence) && alwaysExits(alternative); // <-- must require BOTH
    }
    return false;
}

void DeadCodeChecker::checkUnreachableCode(TSNode blockNode, const ParsedSource& parsedSource, std::vector<Warning>& warnings) {
    uint32_t count = ts_node_child_count(blockNode);
    bool exited = false;

    for (uint32_t i = 0; i < count; ++i) {
        TSNode child = ts_node_child(blockNode, i);
        std::string type = ts_node_type(child);
        if (type == "{" || type == "}" || type == "comment") continue;   // <-- add comment

        if (exited) {
            int line = ts_node_start_point(child).row + 1;
            warnings.push_back({line, "Unreachable Code",
                "This code can never execute because a previous statement in this block "
                "always exits via return/break/continue/goto."});
            break;
        }
        if (alwaysExits(child)) exited = true;
    }
}

// ---------- Check 6: Unused/Dead Functions (unreachable from main) ----------

std::string DeadCodeChecker::getFunctionName(TSNode functionDefNode, const std::string& source) {
    TSNode declarator = ts_node_child_by_field_name(functionDefNode, "declarator", strlen("declarator"));
    if (ts_node_is_null(declarator)) return "";
    return extractIdentifierFromDeclarator(declarator, source);  // reuses your Check 1 helper
}

// ---------- Single traversal, dispatches by node type ----------

void DeadCodeChecker::visitNode(TSNode node, const ParsedSource& parsedSource, std::vector<Warning>& warnings) {
    std::string type = ts_node_type(node);

    if (type == "function_definition") {
        checkUnusedVariables(node, parsedSource, warnings);
    } 
    else if (type == "compound_statement") {
        checkUnreachableCode(node, parsedSource, warnings);
    }

    uint32_t childCount = ts_node_child_count(node);
    for (uint32_t i = 0; i < childCount; ++i) {
        visitNode(ts_node_child(node, i), parsedSource, warnings);
    }
}

// ---------- Analyze Source ----------

std::vector<Warning> DeadCodeChecker::analyzeSource(const ParsedSource& parsedSource) {
    std::vector<Warning> warnings;
    if (parsedSource.tree == nullptr) return warnings;
    TSNode rootNode = ts_tree_root_node(parsedSource.tree);
    visitNode(rootNode, parsedSource, warnings);
    checkUnusedFunctions(parsedSource, warnings);   // <-- new
    return warnings;
}