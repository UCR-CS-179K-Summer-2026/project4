#include "deadCodeChecker.h"
#include <iostream>
#include <string>
#include <cstring>

// ---------- Shared Helpers  ----------

std::string DeadCodeChecker::nodeText(TSNode node, const std::string& source) {
    uint32_t start = ts_node_start_byte(node);
    uint32_t end = ts_node_end_byte(node);
    return source.substr(start, end - start);
}

std::string DeadCodeChecker::extractIdentifierFromDeclarator(TSNode node, const std::string& source) {
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

// ...existing code...
void DeadCodeChecker::checkUnreachableCode(TSNode blockNode, const ParsedSource& parsedSource, std::vector<Warning>& warnings) {
    uint32_t count = ts_node_child_count(blockNode);
    bool exited = false;
    int rangeStart = -1;
    int rangeEnd = -1;

    auto emitRange = [&]() {
        if (rangeStart == -1) return;

        std::string message;
        if (rangeStart == rangeEnd) {
            message =
                "This code can never execute because a previous statement in this block "
                "always exits via return/break/continue/goto. "
                "Unreachable line: " + std::to_string(rangeStart) + ".";
        } else {
            message =
                "This code can never execute because a previous statement in this block "
                "always exits via return/break/continue/goto. "
                "Unreachable lines: " + std::to_string(rangeStart) + "-" +
                std::to_string(rangeEnd) + ".";
        }

        warnings.push_back({
            rangeStart,
            "Unreachable Code",
            message
        });
        rangeStart = -1;
        rangeEnd = -1;
    };

    for (uint32_t i = 0; i < count; ++i) {
        TSNode child = ts_node_child(blockNode, i);
        std::string type = ts_node_type(child);
        if (type == "{" || type == "}" || type == "comment") continue;

        int line = ts_node_start_point(child).row + 1;

        if (exited) {
            if (rangeStart == -1) rangeStart = line;
            rangeEnd = line;
        }

        if (alwaysExits(child)) {
            exited = true;
        } else if (rangeStart != -1 && !exited) {
            emitRange();
        }
    }

    emitRange();
}

// ---------- Check 6: Unused/Dead Functions (unreachable from main) ----------

std::string DeadCodeChecker::getFunctionName(TSNode functionDefNode, const std::string& source) {
    TSNode declarator = ts_node_child_by_field_name(functionDefNode, "declarator", strlen("declarator"));
    if (ts_node_is_null(declarator)) return "";
    return extractIdentifierFromDeclarator(declarator, source);  // reuses your Check 1 helper
}

void DeadCodeChecker::collectCalledFunctionNames(TSNode node, const std::string& source, std::set<std::string>& callees) {
    std::string type = ts_node_type(node);

    if (type == "call_expression") {
        TSNode functionField = ts_node_child_by_field_name(node, "function", strlen("function"));
        if (!ts_node_is_null(functionField) && std::string(ts_node_type(functionField)) == "identifier") {
            callees.insert(nodeText(functionField, source));
        }
        // member calls (obj.foo()), qualified calls (ns::foo()), function pointers,
        // and virtual dispatch are intentionally out of scope, same as your other checks
    }

    uint32_t childCount = ts_node_child_count(node);
    for (uint32_t i = 0; i < childCount; ++i) {
        collectCalledFunctionNames(ts_node_child(node, i), source, callees);
    }
}

void DeadCodeChecker::checkUnusedFunctions(const ParsedSource& parsedSource, std::vector<Warning>& warnings) {
    const std::string& source = parsedSource.source;
    TSNode root = ts_tree_root_node(parsedSource.tree);

    // Gather every top-level function definition: name -> its node
    std::vector<std::pair<std::string, TSNode>> functions;
    uint32_t topCount = ts_node_child_count(root);
    for (uint32_t i = 0; i < topCount; ++i) {
        TSNode child = ts_node_child(root, i);
        if (std::string(ts_node_type(child)) == "function_definition") {
            std::string name = getFunctionName(child, source);
            if (!name.empty()) functions.push_back({name, child});
        }
    }

    // No main -> no reachability root to check against; skip rather than guess.
    bool hasMain = false;
    for (auto& [name, node] : functions) {
        if (name == "main") { hasMain = true; break; }
    }
    if (!hasMain) return;

    // Build the call graph: function name -> set of names it calls
    std::unordered_map<std::string, std::set<std::string>> callGraph;
    for (auto& [name, node] : functions) {
        TSNode body = ts_node_child_by_field_name(node, "body", strlen("body"));
        std::set<std::string> callees;
        if (!ts_node_is_null(body)) collectCalledFunctionNames(body, source, callees);
        callGraph[name] = callees;
    }

    // Reachability from main (iterative DFS; naturally handles recursion via the visited set)
    std::set<std::string> reachable;
    std::vector<std::string> stack = { "main" };
    while (!stack.empty()) {
        std::string current = stack.back();
        stack.pop_back();
        if (reachable.count(current)) continue;
        reachable.insert(current);
        auto it = callGraph.find(current);
        if (it != callGraph.end()) {
            for (auto& callee : it->second) {
                if (!reachable.count(callee)) stack.push_back(callee);
            }
        }
    }

    // Anything defined but never reached is dead
    for (auto& [name, node] : functions) {
        if (name == "main" || reachable.count(name)) continue;
        int line = ts_node_start_point(node).row + 1;
        warnings.push_back({
            line,
            "Unused/Dead Function",
            "Function \"" + name + "\" is never called from main (directly or indirectly) and is dead code."
        });
    }
}

// ---------- Single traversal, dispatches by node type ----------
void DeadCodeChecker::visitNode(TSNode node, const ParsedSource& parsedSource, std::vector<Warning>& warnings) {
    std::string type = ts_node_type(node);

    if (type == "compound_statement") {
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