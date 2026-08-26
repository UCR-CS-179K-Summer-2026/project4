#ifndef COMMENT_CHECKER_H
#define COMMENT_CHECKER_H

#include <iostream>
#include <fstream>
#include <regex>
#include <string>
#include <vector>
#include "../ParsedSource.h"
#include "../Detector.h"
#include "../Parser.h"
#include <tree_sitter/api.h>

class commentChecker : public Detector {
    private:
        Parser parser;
        void visitNode(TSNode node, const ParsedSource& parsedSource, std::vector<Warning>& warnings) override;
        bool scanForComments(TSNode currentNode);
    public: 
        std::vector<Warning> analyzeSource(const ParsedSource& parsedSource) override;
};
#endif