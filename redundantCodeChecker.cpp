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