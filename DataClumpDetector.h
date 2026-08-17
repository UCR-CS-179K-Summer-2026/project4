#ifndef DATA_CLUMP_DETECTOR_H
#define DATA_CLUMP_DETECTOR_H

#include <vector>
#include <map>
#include <string>
#include "ParsedSource.h"
#include "Detector.h"
#include "NameAnalyzer.h"

class DataClumpDetector : public Detector {
    private:
        struct ClumpInfo {
            std::vector<int> lineNumbers;
            int counter = 0;
        };
        NameAnalyzer nameAnalyzer;
        std::map<std::vector<std::string>, ClumpInfo> variableClumps;
        void visitNode(TSNode node, const ParsedSource& parsedSource, std::vector<Warning>& warnings) override;
        void checkFunctionParams(TSNode node, const ParsedSource& parsedSource, std::vector<Warning>& warnings);
        void checkInsideFunction(TSNode node, const ParsedSource& parsedSource, std::vector<Warning>& warnings);
        void checkCallExpression(TSNode node, const ParsedSource& parsedSource, std::vector<Warning>& warnings);
        void checkForDataClumps(std::vector<Warning>& warnings);
    public:
        DataClumpDetector() = default;
        std::vector<Warning> analyzeSource(const ParsedSource& parsedSource) override;
};

#endif // DATA_CLUMP_DETECTOR_H