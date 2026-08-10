#ifndef DEEP_IF_DETECTOR_H
#define DEEP_IF_DETECTOR_H

#include <tree_sitter/api.h>
#include "Detector.h"
#include "ParsedSource.h"
#include "NameAnalyzer.h"

class DeepIfDetector : public Detector {
    private:
        int depth;
        const int MAX_DEPTH = 3;
        void visitNode(TSNode node, const ParsedSource& parsedSource, int& warningCount) override;
        void outputErrorMessage(const std::string& name, const int& line, int& warningCount);
    public:
        DeepIfDetector() {depth = 0;};
        int analyzeSource(const ParsedSource& parsedSource) override;
};

#endif // DEEP_IF_DETECTOR_H