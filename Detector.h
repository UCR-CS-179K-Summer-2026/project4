#ifndef DETECTOR_H
#define DETECTOR_H

#include "ParsedSource.h"
#include <tree_sitter/api.h>
#include <vector>
#include <string>

struct Warning {
    int line;
    std::string category;   // which check found it, e.g. "dead-variable", "redundant-boolean", "chained-if"
    std::string message;
};

class Detector {
    private:
        virtual void visitNode(TSNode node, const ParsedSource& parsedSource, std::vector<Warning>& warnings) = 0;
    public:
        virtual std::vector<Warning> analyzeSource(const ParsedSource& parsedSource) = 0;
        virtual ~Detector() = default;
};

#endif