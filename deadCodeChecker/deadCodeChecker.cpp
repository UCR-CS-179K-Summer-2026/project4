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

    if (type == "identifier" || type == "field_identifier") {
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
        if (childType == "identifier" || childType == "field_identifier") {
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

void DeadCodeChecker::checkUnreachableCode(TSNode blockNode, const ParsedSource& parsedSource, std::vector<Warning>& warnings) {
    const std::string& source = parsedSource.source;

    // If this block's enclosing function uses goto/labels anywhere,
    // our reachability model can't account for jump targets, so never
    // auto-fix in this function -- warnings only.
    TSNode enclosingFunc = findEnclosingFunction(blockNode);
    bool suppressFix = !ts_node_is_null(enclosingFunc) && containsGotoOrLabel(enclosingFunc);

    uint32_t count = ts_node_child_count(blockNode);
    bool exited = false;
    int rangeStart = -1;
    int rangeEnd = -1;
    TSNode rangeStartNode{};
    TSNode rangeEndNode{};

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

        Warning w{ rangeStart, "Unreachable Code", message };

        if (!suppressFix) {
            uint32_t start = ts_node_start_byte(rangeStartNode);
            uint32_t end = ts_node_end_byte(rangeEndNode);

            if (end < source.size() && source[end] == '\n') {
                end += 1;
            } else if (end + 1 < source.size() && source[end] == '\r' && source[end + 1] == '\n') {
                end += 2;
            }

            w.fix = Edit{ start, end, "" };
        }

        warnings.push_back(w);
        rangeStart = -1;
        rangeEnd = -1;
    };

    for (uint32_t i = 0; i < count; ++i) {
        TSNode child = ts_node_child(blockNode, i);
        std::string type = ts_node_type(child);
        if (type == "{" || type == "}" || type == "comment") continue;

        int line = ts_node_start_point(child).row + 1;

        if (exited) {
            if (rangeStart == -1) {
                rangeStart = line;
                rangeStartNode = child;
            }
            rangeEnd = line;
            rangeEndNode = child;
        }

        if (alwaysExits(child)) {
            exited = true;
        } else if (rangeStart != -1 && !exited) {
            emitRange();
        }
    }

    emitRange();
}

// New helper: recursively check if a subtree contains a goto or a label
bool DeadCodeChecker::containsGotoOrLabel(TSNode node) {
    std::string type = ts_node_type(node);
    if (type == "goto_statement" || type == "labeled_statement") {
        return true;
    }
    uint32_t count = ts_node_child_count(node);
    for (uint32_t i = 0; i < count; ++i) {
        if (containsGotoOrLabel(ts_node_child(node, i))) return true;
    }
    return false;
}

// New helper: walk up from any node to its enclosing function_definition
TSNode DeadCodeChecker::findEnclosingFunction(TSNode node) {
    TSNode parent = ts_node_parent(node);
    while (!ts_node_is_null(parent)) {
        if (std::string(ts_node_type(parent)) == "function_definition") {
            return parent;
        }
        parent = ts_node_parent(parent);
    }
    return TSNode{};
}


// ---------- Check 6: Unused/Dead Functions (unreachable from main) ----------

std::string DeadCodeChecker::getFunctionName(TSNode functionDefNode, const std::string& source) {
    TSNode declarator = ts_node_child_by_field_name(functionDefNode, "declarator", strlen("declarator"));
    if (ts_node_is_null(declarator)) return "";
    return extractIdentifierFromDeclarator(declarator, source);  // reuses your Check 1 helper
}

// NEW: pulls parameter names out of a function_definition's parameter_list,
// for PointsToAnalyzer::registerParams. "Animal *x" -> "x", etc.
std::vector<std::string> DeadCodeChecker::extractParamNames(TSNode functionDefNode, const std::string& source) {
    std::vector<std::string> names;
    TSNode declarator = ts_node_child_by_field_name(functionDefNode, "declarator", strlen("declarator"));
    if (ts_node_is_null(declarator)) return names;
 
    // declarator is a function_declarator wrapping the name + parameter_list
    // (possibly itself wrapped in a pointer_declarator for pointer-returning
    // functions -- walk down to the function_declarator the same way
    // extractIdentifierFromDeclarator walks down to the identifier).
    TSNode current = declarator;
    while (!ts_node_is_null(current) && std::string(ts_node_type(current)) != "function_declarator") {
        TSNode inner = ts_node_child_by_field_name(current, "declarator", strlen("declarator"));
        if (ts_node_is_null(inner)) return names;
        current = inner;
    }
    if (ts_node_is_null(current)) return names;

    TSNode params = ts_node_child_by_field_name(current, "parameters", strlen("parameters"));
    if (ts_node_is_null(params)) return names;

    uint32_t count = ts_node_named_child_count(params);
    for (uint32_t i = 0; i < count; ++i) {
        TSNode param = ts_node_named_child(params, i);
        if (std::string(ts_node_type(param)) != "parameter_declaration") continue;
        TSNode paramDeclarator = ts_node_child_by_field_name(param, "declarator", strlen("declarator"));
        if (ts_node_is_null(paramDeclarator)) continue;
        std::string name = extractIdentifierFromDeclarator(paramDeclarator, source);
        if (!name.empty()) names.push_back(name);
    }
    return names;
}

// walks a class_specifier (unwrapping a top-level "declaration" node
// - records its base(s) into hierarchy.bases
// - adds each method's qualified name ("Dog::makeSound") to `functions`
// - records which methods it defines into hierarchy.methodsDefinedIn
void DeadCodeChecker::collectClassMethods(TSNode declNode, const std::string& source,
                                           std::vector<std::pair<std::string, TSNode>>& functions,
                                           ClassHierarchy& hierarchy) {
    TSNode classSpec = declNode;
    if (std::string(ts_node_type(declNode)) == "declaration") {
        TSNode typeField = ts_node_child_by_field_name(declNode, "type", strlen("type"));
        if (ts_node_is_null(typeField)) return;
        classSpec = typeField;
    }
    if (std::string(ts_node_type(classSpec)) != "class_specifier") return;

    TSNode nameNode = ts_node_child_by_field_name(classSpec, "name", strlen("name"));
    if (ts_node_is_null(nameNode)) return;
    std::string className = nodeText(nameNode, source);

    // base_class_clause: "class Dog : public Animal { ... }" -- named
    // children here include an access_specifier ("public") alongside the
    // base type_identifier, so filter to identifiers only.
    TSNode baseClause = ts_node_child_by_field_name(classSpec, "base_class_clause", strlen("base_class_clause"));
    if (!ts_node_is_null(baseClause)) {
        uint32_t bc = ts_node_named_child_count(baseClause);
        for (uint32_t i = 0; i < bc; ++i) {
            TSNode baseNameNode = ts_node_named_child(baseClause, i);
            std::string baseKind = ts_node_type(baseNameNode);
            if (baseKind == "type_identifier" || baseKind == "identifier") {
                hierarchy.bases[className].push_back(nodeText(baseNameNode, source));
            }
            // access_specifier ("public"/"private"/"protected") intentionally skipped
        }
    }

    TSNode body = ts_node_child_by_field_name(classSpec, "body", strlen("body"));
    if (ts_node_is_null(body)) return;

    uint32_t fieldCount = ts_node_named_child_count(body);
    for (uint32_t i = 0; i < fieldCount; ++i) {
        TSNode member = ts_node_named_child(body, i);
        if (std::string(ts_node_type(member)) != "function_definition") continue;

        std::string methodName = getFunctionName(member, source);
        if (methodName.empty()) continue;

        functions.push_back({className + "::" + methodName, member});
        hierarchy.methodsDefinedIn[className].insert(methodName);
    }
}

// Resolves field_expression: calls (a->makeSound()) through points-to analysis
// plain identifier calls (foo()) are unchanged.
void DeadCodeChecker::collectCalledFunctionNames(TSNode node, const std::string& source, const std::string& currentFunction, const PointsToAnalyzer& pta, std::set<std::string>& callees) {
    std::string type = ts_node_type(node);

    if (type == "call_expression") {
        TSNode functionField = ts_node_child_by_field_name(node, "function", strlen("function"));
        if (!ts_node_is_null(functionField)) {
            std::string fieldKind = ts_node_type(functionField);

            if (fieldKind == "identifier") {
                callees.insert(nodeText(functionField, source));

            } else if (fieldKind == "field_expression") {
                // a->makeSound()  or  a.makeSound()
                TSNode objectNode = ts_node_child_by_field_name(functionField, "argument", strlen("argument"));
                TSNode fieldNode = ts_node_child_by_field_name(functionField, "field", strlen("field"));
                if (!ts_node_is_null(objectNode) && !ts_node_is_null(fieldNode) &&
                    std::string(ts_node_type(objectNode)) == "identifier") {
                    std::string methodName = nodeText(fieldNode, source);
                    VarId ptrVar{currentFunction, nodeText(objectNode, source)};
                    std::set<std::string> resolved = pta.resolveVirtualCall(ptrVar, methodName);
                    callees.insert(resolved.begin(), resolved.end());
                }
                // object expr more complex than a bare identifier (chained
                // calls, etc.) -- out of scope, same as qualified ns::foo() calls
            }
        }
    }

    uint32_t childCount = ts_node_child_count(node);
    for (uint32_t i = 0; i < childCount; ++i) {
        collectCalledFunctionNames(ts_node_child(node, i), source, currentFunction, pta, callees);
    }
}

void DeadCodeChecker::checkUnusedFunctions(const ParsedSource& parsedSource, std::vector<Warning>& warnings) {
    const std::string& source = parsedSource.source;
    TSNode root = ts_tree_root_node(parsedSource.tree);

    // Gather every function definition: free functions AND class methods
    // (qualified as "ClassName::methodName"), plus the class hierarchy.
    std::vector<std::pair<std::string, TSNode>> functions;
    ClassHierarchy hierarchy;

    uint32_t topCount = ts_node_child_count(root);
    for (uint32_t i = 0; i < topCount; ++i) {
        TSNode child = ts_node_child(root, i);
        std::string kind = ts_node_type(child);
        if (kind == "function_definition") {
            std::string name = getFunctionName(child, source);
            if (!name.empty()) functions.push_back({name, child});
        } else if (kind == "declaration" || kind == "class_specifier") {
            size_t before = functions.size();
            collectClassMethods(child, source, functions, hierarchy);
        }
    }

    bool hasMain = false;
    for (auto& [name, node] : functions) {
        if (name == "main") { hasMain = true; break; }
    }
    if (!hasMain) return;

    // Points-to pass: register every function/method's params, walk each
    // body to collect flow edges + seeds, then solve to a fixpoint. Must
    // happen before building callGraph, since collectCalledFunctionNames
    // queries the solved points-to sets.
    PointsToAnalyzer pta(hierarchy);
    for (auto& [name, node] : functions) {
        pta.registerParams(name, extractParamNames(node, source));
    }
    for (auto& [name, node] : functions) {
        TSNode body = ts_node_child_by_field_name(node, "body", strlen("body"));
        if (!ts_node_is_null(body)) pta.collectFromFunction(name, body, parsedSource);
    }
    pta.solve();

    // Build the call graph: function name -> set of names it calls.
    // Qualified names ("Dog::makeSound") flow through exactly like
    // unqualified free-function names.
    std::unordered_map<std::string, std::set<std::string>> callGraph;
    for (auto& [name, node] : functions) {
        TSNode body = ts_node_child_by_field_name(node, "body", strlen("body"));
        std::set<std::string> callees;
        if (!ts_node_is_null(body)) collectCalledFunctionNames(body, source, name, pta, callees);
        callGraph[name] = callees;
    }

    // Reachability from main -- unchanged.
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

    // Anything defined but never reached is dead. Constructors/destructors
    // (unqualified name matches class name, e.g. "Dog::Dog") are called
    // implicitly rather than via call_expression, so they'd always show
    // up dead here -- skip them explicitly if your test files include any.
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
    std::cerr << "###### BUILD CHECK: THIS IS THE NEW FILE ######\n";
    std::vector<Warning> warnings;
    if (parsedSource.tree == nullptr) return warnings;
    TSNode rootNode = ts_tree_root_node(parsedSource.tree);
    visitNode(rootNode, parsedSource, warnings);
    checkUnusedFunctions(parsedSource, warnings);
    return warnings;
}