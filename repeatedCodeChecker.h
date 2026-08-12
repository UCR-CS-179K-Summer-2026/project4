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

static constexpr int kMinWindowSize = 3; // Minimum number of lines to consider for repetition
static constexpr int kMaxWindowSize = 10; // Maximum number of lines to consider for repetition
 
TSNode findIDNode(TSNode node) const;
std::string extractFunctionName(TSNode functionDefNode, const std::string& source) const;

std::vector<TSNode> collectStatements(TSNode blockNode) const;

size_t hashSubtree(TSNode node, const std::string& source) const;

bool subtreesEqual(TSNode left, TSNode right, const std:: string& source) const;
 
void reportRepeatedBlock(const std::vector<TSNode>& statements, int windowSize, const std::vector<int>& startIndices,const std::string& functionName, const std::string& source) const;

int findRepeatedBlocks(const std::vector<TSNode>& statements, const std::string& functionName, const std::string& source) const;
void scanBlocksForRepeats(TSNode node, const std::string& functionName, const std::string& source, int& warningCount) const;
};

#endif 