#include "repeatedCodeChecker.h"

#include <iostream>
#include <cstring>
#include <functional>
#include <algorithm>
#include <unordered_map>
#include <tree_sitter/api.h>

//Perform DFS on the tree to find the first identifier node
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

//Extracts the function name given a function_definition node. Uses it's ID Node's
//child declarator node to extract the raw function name.
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


std::vector<RepeatedCodeChecker::codeLine> RepeatedCodeChecker::extractCodeLines(const std::string& content) const{
    std::vector<codeLine> codeLines;
    std::string noBlockComments = stripBlockComments(content);

//Collects all the statements in a compound_statement node, ignoring comments and unnamed nodes. Returns a vector of TSNode representing the statements.
std::vector<TSNode> RepeatedCodeChecker::collectStatements(TSNode blockNode) const{
    std::vector<TSNode> statements;
    uint32_t childCount = ts_node_child_count(blockNode);

    for(uint32_t i =0; i<childCount; ++i){
        TSNode child = ts_node_child(blockNode, i);

        if(ts_node_is_named(child) && strcmp(ts_node_type(child), "comment") !=0){
            statements.push_back(child);
        }
    }
    return statements;
}

//Stores the subtrees into a hashmap. If two subtrees have the same hash, that means they are the same
size_t RepeatedCodeChecker::hashSubtree(TSNode node, const std::string& source) const{
    size_t h = std::hash<std::string>{}(ts_node_type(node));
    uint32_t childCount = ts_node_child_count(node);

        if(childCount == 0){
            uint32_t startByte = ts_node_start_byte(node);
            uint32_t endByte = ts_node_end_byte(node);

            std::string text = source.substr(startByte, endByte - startByte);
            h^= std::hash<std::string>{}(text); + 0x9e3779b9 + (h << 6) + (h >> 2);
            return h;
    }

        for(uint32_t i = 0; i< childCount; ++i){
            size_t childHash = hashSubtree(ts_node_child(node,i), source);
            h^= childHash + 0x9e3779b9 + (h << 6) + (h >> 2);
        }

    return h;
}

//Structural check to compare two subtrees together. This is used to prevent hash collisions.
bool RepeatedCodeChecker::subtreesEqual(TSNode left, TSNode right, const std::string& source) const {
    if (ts_node_is_null(left) || ts_node_is_null(right)) {
        return ts_node_is_null(left) && ts_node_is_null(right);
    }

    if(strcmp (ts_node_type(left), ts_node_type(right)) != 0){
        return false;
    }

    uint32_t leftChildCount = ts_node_child_count(left);
    uint32_t rightChildCount = ts_node_child_count(right);

    if(leftChildCount != rightChildCount){
        return false;
    }

    if(leftChildCount ==0){
        uint32_t leftStartByte = ts_node_start_byte(left);
        uint32_t leftEndByte = ts_node_end_byte(left);

        uint32_t rightStartByte = ts_node_start_byte(right);
        uint32_t rightEndByte = ts_node_end_byte(right);

        return source.compare(leftStartByte, leftEndByte - leftStartByte, source, rightStartByte, rightEndByte - rightStartByte) == 0;

    }

    for(uint32_t i = 0; i< leftChildCount; ++i){
        if(!subtreesEqual(ts_node_child(left,i), ts_node_child(right,i), source)){
            return false;
        }
    }
    return true;
}

//Responsible for printing the warning message to the console when called.
void RepeatedCodeChecker::reportRepeatedBlock(const std::vector<TSNode>& statements, int windowSize, const std::vector<int>& startIndices,const std::string& functionName, const std::string& source) const
{
    std::ostringstream msg;
    msg << "Repeated code detected in function: " << functionName << "\n";

    for (int startIndex : startIndices){
        int firstLine = static_cast<int>(ts_node_start_point(statements[startIndex]).row)+1;
        int lastLine = static_cast<int>(ts_node_end_point(statements[startIndex + windowSize - 1]).row)+1;
 
        std::cout << " In line(s) " << firstLine << " to " << lastLine << " " << std::endl;
    }
 
    std::cout << " Repeated code:" << std::endl;
 
    uint32_t startBlockByte = ts_node_start_byte(statements[startIndices[0]]);
    uint32_t endBlockByte = ts_node_end_byte(statements[startIndices[0] + windowSize - 1]);

    std::cout << source.substr(startBlockByte, endBlockByte - startBlockByte) << std::endl;
}

//Uses the sliding window to find duplicates over one block's statement list
//Start with large window size first then decrease the window size to find smaller duplicates. This is done to avoid overlapping duplicates.
int RepeatedCodeChecker::findRepeatedBlocks(const std::vector<TSNode>& statements, const std::string& functionName, const std::string& source) const {
   int n = static_cast<int>(statements.size());
    int warningCount = 0;
    if (n < kMinWindowSize) {
        return warningCount;
    }
 //Hash each statement once to prevent repeated hashing of the same statement.
    std::vector<size_t> stmtHash(n);
    for (int i = 0; i < n; ++i) {
        stmtHash[i] = hashSubtree(statements[i], source);
    }
 
    std::vector<bool> covered(n, false);//If statement is already covered, store in this vector
    int maxWindow = std::min(kMaxWindowSize, n);
 
 //Builds a combined has for every possible window of this size and skips any window that is already covered by 
 //A previously found larger duplicate.
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
 
 //Sort the entries in blockMap by the first occurrence of the repeated block to ensure consistent output order.
        std::vector<std::pair<size_t, std::vector<int>>> ordered(blockMap.begin(), blockMap.end());
        std::sort(ordered.begin(), ordered.end(), [](const auto& a, const auto& b) {
            return a.second.front() < b.second.front();
        });
 
        for (auto& entry : ordered) {
            std::vector<int>& starts = entry.second;
            if (starts.size() < 2) continue;//If only one occurance, its not a repeat
 
            std::vector<int> nonOverlapping;//Only keep non-overlapping repeats and then verify each one structurally against the first kept occurance
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
                    if (!matches) continue;//Handles hash collisions, if the subtrees dont equal but have the same hash, skip
                }
 
                nonOverlapping.push_back(s);
                lastEnd = s + windowSize - 1;
            }
 
            if (nonOverlapping.size() < 2) continue;
 
            reportRepeatedBlock(statements, windowSize, nonOverlapping, functionName, source);
            ++warningCount;
 
            for (int s : nonOverlapping) {
                for (int i = 0; i < windowSize; ++i) {
                    covered[s + i] = true;
                }
            }
        }
    }
 
    return warningCount;
}

//Recursively scans the AST for compound_statement nodes. For each compound_statement node, it collects the statements and calls findRepeatedBlocks to check for repeated code blocks.
void RepeatedCodeChecker::scanBlocksForRepeats(TSNode node, const std::string& functionName, const std::string& source, int& warningCount) const{
    if(ts_node_is_null(node)){
        return;
    }

    const char* type = ts_node_type(node);

    if(strcmp(type, "function_definition") == 0){
        return;
    }

    if(strcmp(type, "compound_statement") == 0){
        std::vector<TSNode> statements = collectStatements(node);
        warningCount += findRepeatedBlocks(statements, functionName, source);
    }

    uint32_t childCount = ts_node_child_count(node);

    for(uint32_t i = 0; i< childCount; ++i){
        scanBlocksForRepeats(ts_node_child(node,i), functionName, source, warningCount);
    }
}

//Walks the AST and finds all function_definition nodes. For each function_definition node, it extracts the function name and scans the body of the function for repeated code blocks.
void RepeatedCodeChecker::visitNode(TSNode node, const ParsedSource& parsedSource, int& warningCount) {
    if (ts_node_is_null(node)) {
        return;
    }
 
    if (strcmp(ts_node_type(node), "function_definition") == 0) {
        int startLine = static_cast<int>(ts_node_start_point(node).row) + 1;
        int endLine = static_cast<int>(ts_node_end_point(node).row) + 1;
 
        std::string functionName = extractFunctionName(node, parsedSource.source);
        if (functionName.empty()) {
            functionName = "<unnamed function>";
        }

        TSNode bodyNode = ts_node_child_by_field_name(node, "body", strlen("body"));
        if(!ts_node_is_null(bodyNode)){
            scanBlocksForRepeats(bodyNode, functionName, parsedSource.source, warningCount);
        }

        return;
    }

        uint32_t childCount = ts_node_child_count(node);

        for(uint32_t i =0; i< childCount; ++i){
            visitNode(ts_node_child(node,i), parsedSource, warningCount);
        }
}

//Analyzes the parsed source code for repeated code blocks. It starts by checking if the parse tree is valid, then traverses the tree to find function definitions and analyze their bodies for repeated code.
int RepeatedCodeChecker::analyzeSource(const ParsedSource& source){
    if(source.tree == nullptr){
        return 0;
    }

    int warningCount = 0;
    TSNode rootNode = ts_tree_root_node(source.tree);
    visitNode(rootNode, source, warningCount);

    return warningCount;

}
