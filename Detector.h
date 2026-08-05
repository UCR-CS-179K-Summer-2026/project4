#ifndef DETECTOR_H
#define DETECTOR_H

#include "ParsedSource.h"
#include <tree_sitter/api.h>

class Detector {
    private:
        virtual void visitNode(TSNode node, const ParsedSource& parsedSource, int& warningCount) = 0;
    public:
        virtual ~Detector() = default;
        virtual int analyzeSource(const ParsedSource& parsedSource) = 0;

};

#endif