#include "repeatedCodeChecker.h"

#include <iostream>
#include <sstream>
#include <regex>
#include <algorithm>
#include <unordered_map>

std::string RepeatedCodeChecker::stripBlockComments(const std::string& content) const {
    static const std::regex blockCommentRegex(R"(/\*[\s\S]*?\*/)");
    std::string result;
    result.reserve(content.size());

    auto begin = std::sregex_iterator(content.begin(), content.end(), blockCommentRegex);

    auto end = std::sregex_iterator();
    size_t lastPos = 0;

    for(auto it = begin; it !=end; ++it){
        std::smatch match = *it;
        result.append(content, lastPos, match.position() - lastPos);

        std::string matched = match.str();
        long newLineCount = std::count(matched.begin(), matched.end(), '\n');
        result.append(static_cast<size_t>(newLineCount), '\n');

        lastPos = match.position(0) + match.length(0);

    }

    result. append(content, lastPos, content.size() - lastPos);
    return result;

}

std::string RepeatedCodeChecker::stripLineComment(const std::string& line) const{
    static const std::regex lineCommentRegex(R"(//.*$)");
    return std::regex_replace(line, lineCommentRegex, "");
}

std::string RepeatedCodeChecker::normalizeWhitespace(const std::string& line) const{
    static const std::regex whitespaceRegex(R"(\s+)");
    std::string collapsed = std::regex_replace(line, whitespaceRegex, " ");

    size_t startPos = collapsed.find_first_not_of(" ");
    if (startPos == std::string::npos) {
        return "";
    }

    size_t endPos = collapsed.find_last_not_of(" ");
    return collapsed.substr(startPos, endPos - startPos + 1);
}

bool RepeatedCodeChecker::isStructuralOnly(const std::string& line) const{
    static const std::regex structuralRe(R"(^[{};]*$)");
    return std::regex_match(line, structuralRe);
}

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

void RepeatedCodeChecker::reportRepeatedBlock(const std::vector<codeLine>& lines, int windowSize, const std::vector<int>& startIndices, const std::string& functionName, std::vector<Warning>& warnings) const
{
    std::ostringstream msg;
    msg << "Repeated code detected in function: " << functionName << "\n";

    for (int startIndex : startIndices){
        int firstLine = lines[startIndex].lineNumber;
        int lastLine = lines[startIndex + windowSize - 1].lineNumber;

        msg << firstLine << " to " << lastLine << " \n";
    }

    msg << " Repeated code:\n";

    for (int i = 0 ; i < windowSize; ++i){
        msg << lines[startIndices[0] + i].text << "\n";
    }

    int firstOccurrenceLine = lines[startIndices[0]].lineNumber;

    warnings.push_back({
        firstOccurrenceLine,
        "Repeated Code",
        msg.str()
    });
}

void RepeatedCodeChecker::findRepeatedBlocks(const std::vector<codeLine>& codeLines, const std::string& functionName, std::vector<Warning>& warnings) const {
    int n = static_cast<int>(codeLines.size());

    if(n < kMinWindowSize){
        return;
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
 
            reportRepeatedBlock(codeLines, windowSize, nonOverlapping, functionName, warnings);
 
            for(int s : nonOverlapping){
                for(int i = 0; i< windowSize; ++i){
                    covered[s + i] = true;
                }
            }
        }
    }
}

void RepeatedCodeChecker::visitNode(TSNode node, const ParsedSource& parsedSource, std::vector<Warning>& warnings) {
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
 
        findRepeatedBlocks(functionLines, functionName, warnings);
        return; 
    }
 
    uint32_t childCount = ts_node_child_count(node);
    for (uint32_t i = 0; i < childCount; ++i) {
        visitNode(ts_node_child(node, i), parsedSource, warnings);
    }
}

std::vector<Warning> RepeatedCodeChecker::analyzeSource(const ParsedSource& parsedSource){
    std::vector<Warning> warnings;

    if (parsedSource.tree == nullptr){
        return warnings;
    }

    allCodeLines = extractCodeLines(parsedSource.source);

    TSNode rootNode = ts_tree_root_node(parsedSource.tree);
    visitNode(rootNode, parsedSource, warnings);

    return warnings;
}