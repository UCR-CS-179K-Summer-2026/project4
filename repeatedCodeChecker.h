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
    int analyzeSource(const ParsedSource& source) override;

private:
    struct codeLine{
        std::string text;
        int lineNumber;
    };
    void visitNode(TSNode node, const ParsedSource& parsedSource, int& warningCount) override;

static constexpr int kMinWindowSize = 3; // Minimum number of lines to consider for repetition
static constexpr int kMaxWindowSize = 10; // Maximum number of lines to consider for repetition

std::string stripBlockComments(const std::string& content) const;
std::string stripLineComment(const std::string& line) const;
std::string normalizeWhitespace(const std::string& line) const;
bool isStructuralOnly(const std::string& line) const;
 
std::vector<codeLine> extractCodeLines(const std::string& content) const;
 
void reportRepeatedBlock(const std::vector<codeLine>& lines,int windowSize,const std::vector<int>& startIndices) const;


};

#endif 