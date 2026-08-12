#ifndef FUNCTION_ANALYZER_H
#define FUNCTION_ANALYZER_H

#include "ParsedSource.h"
#include "NameAnalyzer.h"
#include <tree_sitter/api.h>

class FunctionAnalyzer {
    private:
        NameAnalyzer nameAnalyzer;
    public:
        void checkFunctionName(TSNode node, const ParsedSource& parsedSource, std::vector<Warning>& warnings);
        void checkParameterNames(TSNode node, const ParsedSource& parsedSource, std::vector<Warning>& warnings);
};

#endif // FUNCTION_ANALYZER_H