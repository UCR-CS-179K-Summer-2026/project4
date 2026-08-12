#include "DeepIfDetector.h"
#include "NameAnalyzer.h"

#include <iostream>
#include <cstring>

void DeepIfDetector::outputErrorMessage(const std::string& name, const int& line, int& warningCount) {
    std::cout << "Warning: " << name << " at line " << line << std::endl;
    warningCount++;
}

void DeepIfDetector::visitNode(TSNode node, const ParsedSource& parsedSource, int& warningCount) {
    if(ts_node_is_null(node)) {
        return;
    }

    const char* nodeType = ts_node_type(node);
    if(strcmp(nodeType, "if_statement") == 0) {
        depth++;
        if(depth > MAX_DEPTH) {
            int line = NameAnalyzer().getLineNumber(parsedSource, node);
            outputErrorMessage("Deeply nested if statement", line, warningCount);
        }
    }

    uint32_t childCount = ts_node_child_count(node);
    for(uint32_t i = 0; i < childCount; ++i) {
        TSNode child = ts_node_child(node, i);
        visitNode(child, parsedSource, warningCount);
    }

    if(strcmp(nodeType, "if_statement") == 0) {
        depth--;
    }
}

int DeepIfDetector::analyzeSource(const ParsedSource& parsedSource) {
    TSNode rootNode = ts_tree_root_node(parsedSource.tree);
    int warningCount = 0;
    visitNode(rootNode, parsedSource, warningCount);

    return warningCount;
}