#ifndef REPEATED_CODE_CHECKER_H
#define REPEATED_CODE_CHECKER_H

#include <fstream>
#include <vector>
#include <string>
#include "ParsedSource.h"
#include "Detector.h"

class RepeatedCodeChecker: public Detector{

public:
    RepeatedCodeChecker() = default;
    std::vector<Warning> RepeatedCodeChecker::analyzeSource(const ParsedSource& parsedSource) override;

private:
    struct codeLine{
        std::string text;
        int lineNumber;
    };
    void RepeatedCodeChecker::visitNode(TSNode node, const ParsedSource& parsedSource, std::vector<Warning>& warnings) override;

std::vector<codeLine> allCodeLines;
static constexpr int kMinWindowSize = 3; // Minimum number of lines to consider for repetition
static constexpr int kMaxWindowSize = 10; // Maximum number of lines to consider for repetition

std::string stripBlockComments(const std::string& content) const;
std::string stripLineComment(const std::string& line) const;
std::string normalizeWhitespace(const std::string& line) const;
bool isStructuralOnly(const std::string& line) const;

TSNode findIDNode(TSNode node) const;
std::string extractFunctionName(TSNode functionDefNode, const std::string& source) const;


std::vector<codeLine> extractCodeLines(const std::string& content) const;

void RepeatedCodeChecker::reportRepeatedBlock(const std::vector<codeLine>& lines, int windowSize, const std::vector<int>& startIndices, const std::string& functionName, std::vector<Warning>& warnings) const;
void RepeatedCodeChecker::findRepeatedBlocks(const std::vector<codeLine>& codeLines, const std::string& functionName, std::vector<Warning>& warnings) const;
};

#endif 