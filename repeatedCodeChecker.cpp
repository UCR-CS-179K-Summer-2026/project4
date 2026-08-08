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
 
void RepeatedCodeChecker::reportRepeatedBlock(const std::vector<codeLine>& lines,int windowSize,const std::vector<int>& startIndices) const{
    std::cout << "Warning: Repeated code detected in lines: ";

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

int RepeatedCodeChecker::findRepeatedBlocks(const std::vector<codeLine>& lines) const{
    int n = static_cast<int>(lines.size());

    int warningCount = 0;

    if(n < kMinWindowSize){
        return warningCount;
    }

    std::vector<bool> covered(n,false);
    int maxWindow = std::min(kMaxWindowSize, n);

    for(int windowSize = maxWindow; windowSize >= kMinWindowSize; --windowSize){
        std::unordered_map<std::string, std::vector<int>> blockMap;

        for(int start = 0; start+windowSize <= n; ++start){
            bool anyCovered = false;

            for(int i = 0; i<windowSize; ++i){
                if(covered[start + i]){
                    anyCovered = true;
                    break;
                }
            }

            if(anyCovered){
                continue;
            }

            std::string key;

            for(int i = 0; i<windowSize; ++i){
                key += lines[start + i].text + "\n";
            }

            blockMap[key].push_back(start);
        }

        std::vector <std::pair<std::string, std::vector<int>>> ordered(blockMap.begin(), blockMap.end());
        std::sort(ordered.begin(), ordered.end(), [](const auto& a, const auto& b){
            return a.second.front() < b.second.front();
        });

        for(auto& entry : ordered){
            std::vector<int>& starts = entry.second;
            if(starts.size() <2){
                continue;
            }

            std::vector<int> nonOverlapping;
            int lastEnd = -1;

            for(int s : starts){
                if(s>lastEnd){
                    nonOverlapping.push_back(s);
                    lastEnd = s+ windowSize -1;
                }
            }

            if(nonOverlapping.size() <2){
                continue;
            }


            reportRepeatedBlock(lines, windowSize, nonOverlapping);
            ++warningCount;

            for(int s: nonOverlapping){
                for(int i = 0; i<windowSize; ++i){
                    covered[s + i] = true;
                }
            }
        }
    }

    return warningCount;
}


void RepeatedCodeChecker::visitNode(TSNode node, const ParsedSource& parsedSource, int& warningCount) {
    if(ts_node_is_null(node)){
        return;
    }

    if(strcmp(ts_node_type(node), "function_definition") == 0){
        int startLine = static_cast<int>(ts_node_start_point(node).row)+1;
        int endLine = static_cast<int>(ts_node_end_point(node).row)+1;

        std::vector<codeLine> functionLines;

        for(const auto& line :allCodeLines){
            if(line.lineNumber >= startLine && line.lineNumber <= endLine){
                functionLines.push_back(line);
            }
        }

        warningCount += findRepeatedBlocks(functionLines);
        return;
    }
    uint32_t childCount = ts_node_child_count(node);
    for(uint32_t i=0; i< childCount; ++i){
        visitNode(ts_node_child(node, i), parsedSource, warningCount);
    }
}

int RepeatedCodeChecker::analyzeSource(const ParsedSource& parsedSource){
    std::vector<codeLine> codeLines = extractCodeLines(parsedSource.source);
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

        reportRepeatedBlock(codeLines, windowSize, nonOverlapping);
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