#include "repeatedCodeChecker.h"

#include <iostream>
#include <sstream>
#include <cstring>
#include <functional>
#include <algorithm>
#include <unordered_map>
#include <tree_sitter/api.h>

// Perform DFS on the tree to find the first identifier node
TSNode RepeatedCodeChecker::findIDNode(TSNode node) const {
    if (ts_node_is_null(node)) {
        return node;
    }

    if (strcmp(ts_node_type(node), "identifier") == 0) {
        return node;
    }

    uint32_t childCount = ts_node_child_count(node);
    for (uint32_t i = 0; i < childCount; ++i) {
        TSNode child = ts_node_child(node, i);
        TSNode result = findIDNode(child);
        if (!ts_node_is_null(result)) {
            return result;
        }
    }

    return {};
}

// Extracts the function name given a function_definition node. Uses its ID node's
// child declarator node to extract the raw function name.
std::string RepeatedCodeChecker::extractFunctionName(TSNode functionDefNode, const std::string& source) const {
    TSNode declaratorNode = ts_node_child_by_field_name(functionDefNode, "declarator", strlen("declarator"));
    if (ts_node_is_null(declaratorNode)) {
        return "";
    }

    TSNode identifierNode = findIDNode(declaratorNode);
    if (ts_node_is_null(identifierNode)) {
        return "";
    }

    uint32_t startByte = ts_node_start_byte(identifierNode);
    uint32_t endByte = ts_node_end_byte(identifierNode);
    return source.substr(startByte, endByte - startByte);
}

// Collects all the statements in a compound_statement node, ignoring comments and unnamed nodes.
std::vector<TSNode> RepeatedCodeChecker::collectStatements(TSNode blockNode) const {
    std::vector<TSNode> statements;
    uint32_t childCount = ts_node_child_count(blockNode);

    for (uint32_t i = 0; i < childCount; ++i) {
        TSNode child = ts_node_child(blockNode, i);

        if (ts_node_is_named(child) && strcmp(ts_node_type(child), "comment") != 0) {
            statements.push_back(child);
        }
    }
    return statements;
}

// Hashes a subtree structurally so identical statements produce identical hashes.
size_t RepeatedCodeChecker::hashSubtree(TSNode node, const std::string& source) const {
    size_t h = std::hash<std::string>{}(ts_node_type(node));
    uint32_t childCount = ts_node_child_count(node);

    if (childCount == 0) {
        uint32_t startByte = ts_node_start_byte(node);
        uint32_t endByte = ts_node_end_byte(node);

        std::string text = source.substr(startByte, endByte - startByte);
        h ^= std::hash<std::string>{}(text) + 0x9e3779b9 + (h << 6) + (h >> 2);
        return h;
    }

    for (uint32_t i = 0; i < childCount; ++i) {
        size_t childHash = hashSubtree(ts_node_child(node, i), source);
        h ^= childHash + 0x9e3779b9 + (h << 6) + (h >> 2);
    }

    return h;
}

// Structural check to compare two subtrees together. This is used to prevent hash collisions.
bool RepeatedCodeChecker::subtreesEqual(TSNode left, TSNode right, const std::string& source) const {
    if (ts_node_is_null(left) || ts_node_is_null(right)) {
        return ts_node_is_null(left) && ts_node_is_null(right);
    }

    if (strcmp(ts_node_type(left), ts_node_type(right)) != 0) {
        return false;
    }

    uint32_t leftChildCount = ts_node_child_count(left);
    uint32_t rightChildCount = ts_node_child_count(right);

    if (leftChildCount != rightChildCount) {
        return false;
    }

    if (leftChildCount == 0) {
        uint32_t leftStartByte = ts_node_start_byte(left);
        uint32_t leftEndByte = ts_node_end_byte(left);

        uint32_t rightStartByte = ts_node_start_byte(right);
        uint32_t rightEndByte = ts_node_end_byte(right);

        return source.compare(leftStartByte, leftEndByte - leftStartByte, source, rightStartByte, rightEndByte - rightStartByte) == 0;
    }

    for (uint32_t i = 0; i < leftChildCount; ++i) {
        if (!subtreesEqual(ts_node_child(left, i), ts_node_child(right, i), source)) {
            return false;
        }
    }
    return true;
}

// Builds one Warning describing a repeated block: every line-range it occurs at, plus the code itself.
void RepeatedCodeChecker::reportRepeatedBlock(const std::vector<TSNode>& statements, const std::string& source, int windowSize, const std::vector<int>& startIndices, const std::string& functionName, std::vector<Warning>& warnings) const
{
    std::ostringstream msg;
    msg << "Repeated code detected in function: " << functionName << "\n";

    for (int startIndex : startIndices) {
        int firstLine = static_cast<int>(ts_node_start_point(statements[startIndex]).row) + 1;
        int lastLine = static_cast<int>(ts_node_end_point(statements[startIndex + windowSize - 1]).row) + 1;
        msg << "In line(s) " << firstLine << " to " << lastLine << "\n";
    }

    msg << "Repeated code:\n";

    uint32_t startBlockByte = ts_node_start_byte(statements[startIndices[0]]);
    uint32_t endBlockByte = ts_node_end_byte(statements[startIndices[0] + windowSize - 1]);
    msg << source.substr(startBlockByte, endBlockByte - startBlockByte) << "\n";
    int firstOccurrenceLine = static_cast<int>(ts_node_start_point(statements[startIndices[0]]).row) + 1;

    warnings.push_back({
        firstOccurrenceLine,
        "repeated-code",
        msg.str()
    });
}

// Sliding window over one block's statement list, largest window first, to find repeated runs of statements.
void RepeatedCodeChecker::findRepeatedBlocks(const std::vector<TSNode>& statements, const std::string& functionName, const std::string& source, std::vector<Warning>& warnings) const {
    int n = static_cast<int>(statements.size());
    if (n < kMinWindowSize) {
        return;
    }

    // Hash each statement once to prevent repeated hashing of the same statement.
    std::vector<size_t> stmtHash(n);
    for (int i = 0; i < n; ++i) {
        stmtHash[i] = hashSubtree(statements[i], source);
    }

    std::vector<bool> covered(n, false);
    int maxWindow = std::min(kMaxWindowSize, n);

    for (int windowSize = maxWindow; windowSize >= kMinWindowSize; --windowSize) {
        std::unordered_map<size_t, std::vector<int>> blockMap;

        for (int start = 0; start + windowSize <= n; ++start) {
            bool anyCovered = false;
            for (int i = 0; i < windowSize; ++i) {
                if (covered[start + i]) { anyCovered = true; break; }
            }
            if (anyCovered) continue;

            size_t key = 0;
            for (int i = 0; i < windowSize; ++i) {
                key ^= stmtHash[start + i] + 0x9e3779b97f4a7c15ULL + (key << 6) + (key >> 2);
            }

            blockMap[key].push_back(start);
        }

        // Sort by first occurrence to keep output order consistent.
        std::vector<std::pair<size_t, std::vector<int>>> ordered(blockMap.begin(), blockMap.end());
        std::sort(ordered.begin(), ordered.end(), [](const auto& a, const auto& b) {
            return a.second.front() < b.second.front();
        });

        for (auto& entry : ordered) {
            std::vector<int>& starts = entry.second;
            if (starts.size() < 2) continue; // Only one occurrence isn't a repeat

            std::vector<int> nonOverlapping;
            int lastEnd = -1;

            for (int s : starts) {
                if (s <= lastEnd) continue;

                if (!nonOverlapping.empty()) {
                    int firstStart = nonOverlapping.front();
                    bool matches = true;
                    for (int i = 0; i < windowSize; ++i) {
                        if (!subtreesEqual(statements[firstStart + i], statements[s + i], source)) {
                            matches = false;
                            break;
                        }
                    }
                    if (!matches) continue; // Hash collision, not an actual match
                }

                nonOverlapping.push_back(s);
                lastEnd = s + windowSize - 1;
            }

            if (nonOverlapping.size() < 2) continue;

            reportRepeatedBlock(statements, source, windowSize, nonOverlapping, functionName, warnings);

            for (int s : nonOverlapping) {
                for (int i = 0; i < windowSize; ++i) {
                    covered[s + i] = true;
                }
            }
        }
    }
}

// Recursively scans the AST for compound_statement nodes and checks each for repeated statement runs.
void RepeatedCodeChecker::scanBlocksForRepeats(TSNode node, const std::string& functionName, const std::string& source, std::vector<Warning>& warnings) const {
    if (ts_node_is_null(node)) {
        return;
    }

    const char* type = ts_node_type(node);

    if (strcmp(type, "function_definition") == 0) {
        return;
    }

    if (strcmp(type, "compound_statement") == 0) {
        std::vector<TSNode> statements = collectStatements(node);
        findRepeatedBlocks(statements, functionName, source, warnings);
    }

    uint32_t childCount = ts_node_child_count(node);
    for (uint32_t i = 0; i < childCount; ++i) {
        scanBlocksForRepeats(ts_node_child(node, i), functionName, source, warnings);
    }
}

// Walks the AST and finds all function_definition nodes. For each, extracts the function name
// and scans the body of the function for repeated code blocks.
void RepeatedCodeChecker::visitNode(TSNode node, const ParsedSource& parsedSource, std::vector<Warning>& warnings) {
    if (ts_node_is_null(node)) {
        return;
    }

    if (strcmp(ts_node_type(node), "function_definition") == 0) {
        std::string functionName = extractFunctionName(node, parsedSource.source);
        if (functionName.empty()) {
            functionName = "<unnamed function>";
        }

        TSNode bodyNode = ts_node_child_by_field_name(node, "body", strlen("body"));
        if (!ts_node_is_null(bodyNode)) {
            scanBlocksForRepeats(bodyNode, functionName, parsedSource.source, warnings);
        }

        return;
    }

    uint32_t childCount = ts_node_child_count(node);
    for (uint32_t i = 0; i < childCount; ++i) {
        visitNode(ts_node_child(node, i), parsedSource, warnings);
    }
}

// Analyzes the parsed source code for repeated code blocks: checks the parse tree is valid, then
// traverses it to find function definitions and analyze their bodies for repeated code.
std::vector<Warning> RepeatedCodeChecker::analyzeSource(const ParsedSource& parsedSource) {
    std::vector<Warning> warnings;

    if (parsedSource.tree == nullptr) {
        return warnings;
    }

    TSNode rootNode = ts_tree_root_node(parsedSource.tree);
    visitNode(rootNode, parsedSource, warnings);

    return warnings;
}

//Outputs the refactored message, showing a before and after of the detected code block, with refactor suggestion
std::string RepeatedCodeChecker::offerRefactoring(const std::vector<TSNode>& statements, int windowSize, const std::vector<int>& startIndices, const std::string& functionName, const std::string& source) const{
    uint32_t startByte = ts_node_start_byte(statements[startIndices[0]]);
    uint32_t endByte = ts_node_end_byte(statements[startIndices[0]+windowSize -1]);
    std::string repeatedText = source.substr(startByte, endByte- startByte);

    int firstLine = static_cast<int>(ts_node_start_point(statements[startIndices[0]]).row)+1;
    std::string helperName = functionName + "Helper" + std::to_string(firstLine);

    std::ostringstream out;
    out << "\nSuggested refactor: extract this repeated block and turn into a helper function.\n";
    out << "\nExample:\n\n";
    out << "void " << helperName << "(/* add parameters as needed */) {\n";
    out << "    " << repeatedText << "\n";
    out << "}\n";
    out << "\nThen replace each repeated occurrence with a single call:\n";
    out << "    " << helperName << "(/* matching arguments */);\n";
    out << "\nNote: this is a suggestion for function: " << functionName << " only";
 
    return out.str();
}
