#include "DeepIfDetector.h"
#include "NameAnalyzer.h"

#include <iostream>
#include <cstring>

void DeepIfDetector::visitNode(TSNode node, const ParsedSource& parsedSource, std::vector<Warning>& warnings) {
    if(ts_node_is_null(node)) {
        return;
    }

    const char* nodeType = ts_node_type(node);
    if(strcmp(nodeType, "if_statement") == 0) {
        depth++;
        if(depth > MAX_DEPTH) {
            int line = NameAnalyzer().getLineNumber(parsedSource, node);
            warnings.push_back({
                line,
                "deep-if",
                "Deeply nested if statement. Consider refactoring this code as a function or using early returns to reduce nesting.",
            });
        }
    }

    uint32_t childCount = ts_node_child_count(node);
    for(uint32_t i = 0; i < childCount; ++i) {
        TSNode child = ts_node_child(node, i);
        visitNode(child, parsedSource, warnings);
    }

    if(strcmp(nodeType, "if_statement") == 0) {
        depth--;
    }
}

std::vector<Warning> DeepIfDetector::analyzeSource(const ParsedSource& parsedSource) {
    TSNode rootNode = ts_tree_root_node(parsedSource.tree);
    std::vector<Warning> warnings;
    visitNode(rootNode, parsedSource, warnings);

    return warnings;
}