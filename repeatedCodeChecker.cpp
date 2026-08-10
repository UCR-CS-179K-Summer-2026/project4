#include "repeatedCodeChecker.h"

#include <iostream>
#include <sstream>
#include <regex>
#include <algorithm>
#include <unordered_map>

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

    std::istringstream stream(noBlockComments);
    std::string rawLine;

    int lineNumber = 0;

    while(std::getline(stream, rawLine)){
        lineNumber++;
        std::string noLineComment = stripLineComment(rawLine);
        std::string normalized = normalizeWhitespace(noLineComment);

        if(!normalized.empty() && !isStructuralOnly(normalized)){
            codeLines.push_back({normalized, lineNumber});
        }

        continue;
    }
    return codeLines;
}
 
void RepeatedCodeChecker::reportRepeatedBlock(const std::vector<codeLine>& lines,int windowSize,const std::vector<int>& startIndices, const std::string& functionName) const
{
    std::cout << "Warning: Repeated code detected in function: " << functionName << std::endl;

    for (int startIndex : startIndices){
        int firstLine = lines[startIndex].lineNumber;
        int lastLine = lines[startIndex + windowSize - 1].lineNumber;
 
        std::cout << firstLine << " to " << lastLine << " " << std::endl;
    }
 
    std::cout << " Repeated code:" << std::endl;
 
    for (int i = 0 ; i < windowSize; ++i){
        std::cout << lines[startIndices[0] + i].text << std::endl;
    }
 
    std::cout << std::endl;
}

int RepeatedCodeChecker::findRepeatedBlocks(const std::vector<codeLine>& codeLines, const std::string& functionName) const {
    int n = static_cast<int>(codeLines.size());
 
    int warningCOunt = 0;
    if(n < kMinWindowSize){
        return warningCOunt;
    }
 
    std::vector<bool> covered(n,false);
    int maxWindow = std::min(kMaxWindowSize, n);
 
    for (int windowSize = maxWindow; windowSize >= kMinWindowSize; --windowSize){
        std::unordered_map<std::string, std::vector<int>> blockMap;
 
        for (int start = 0; start+windowSize <= n; ++start){
            bool anyCovered = false;
 
            for(int i =0; i < windowSize; ++i){
                if(covered[start + i]){
                    anyCovered = true;
                    break;
                }
            }
 
            if (anyCovered){
                continue;
            }
 
            std::string key;
            for(int i = 0; i< windowSize; ++i){
                key += codeLines[start + i].text + "\n";
            }
 
            blockMap[key].push_back(start);
        }
 
        std::vector <std::pair<std::string,std::vector<int>>> ordered(blockMap.begin(), blockMap.end());
 
        std::sort(ordered.begin(),ordered.end(), [](const auto& a, const auto& b){
            return a.second.front() < b.second.front();
        });
 
        for(auto& entry : ordered){
            std::vector<int>& starts = entry.second;
            if(starts.size() <2) {
                continue;
            }
 
            std::vector<int> nonOverlapping;
            int lastEnd = -1;
 
            for(int s : starts){
                if(s>lastEnd) {
                    nonOverlapping.push_back(s);
                    lastEnd = s + windowSize - 1;
                }
            }
            if(nonOverlapping.size() <2){
                continue;
            }
 
            reportRepeatedBlock(codeLines, windowSize, nonOverlapping, functionName);
            ++warningCOunt;
 
            for(int s : nonOverlapping){
                for(int i = 0; i< windowSize; ++i){
                    covered[s + i] = true;
                }
            }
        }
    }
    return warningCOunt;
}

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
 
        std::vector<codeLine> functionLines;
        for (const auto& line : allCodeLines) {
            if (line.lineNumber >= startLine && line.lineNumber <= endLine) {
                functionLines.push_back(line);
            }
        }
 
        warningCount += findRepeatedBlocks(functionLines, functionName);
        return; 
    }
 
    uint32_t childCount = ts_node_child_count(node);
    for (uint32_t i = 0; i < childCount; ++i) {
        visitNode(ts_node_child(node, i), parsedSource, warningCount);
    }
}

int RepeatedCodeChecker::analyzeSource(const ParsedSource& parsedSource){
    if (parsedSource.tree == nullptr){
        return 0;
    }

    allCodeLines = extractCodeLines(parsedSource.source);

    int warningCount = 0;
    TSNode rootNode = ts_tree_root_node(parsedSource.tree);
    visitNode(rootNode, parsedSource, warningCount);

    return warningCount;
}