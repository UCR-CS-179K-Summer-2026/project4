#ifndef DATA_CLUMP_DETECTOR_H
#define DATA_CLUMP_DETECTOR_H

#include <vector>
#include <map>
#include <string>
#include <unordered_set>
#include <unordered_map>
#include "ParsedSource.h"
#include "Detector.h"
#include "NameAnalyzer.h"
#include <bitset>
#include "DataClumpNameGen.h"
#include <cstring>

class DataClumpDetector : public Detector {
    private:
        struct ClumpInfo {
            std::vector<int> lineNumbers;
            int counter = 0;
        };
        static const int MAX_GROUPS = 1024;
        std::unordered_map<std::string, std::unordered_map<std::string, int>> variableGraphMap;
        NameAnalyzer nameAnalyzer;
        std::map<std::vector<std::string>, ClumpInfo> variableClumps;
        std::vector<std::unordered_set<std::string>> variableGroups;
        std::vector<int> groupLineNumbers;
        std::unordered_map<std::string, std::bitset<MAX_GROUPS>> variableBitsets;
        DataClumpNameGen nameGenerator;

        void visitNode(TSNode node, const ParsedSource& parsedSource, std::vector<Warning>& warnings) override;
        void checkFunctionParams(TSNode node, const ParsedSource& parsedSource, std::vector<Warning>& warnings);
        void checkInsideFunction(TSNode node, const ParsedSource& parsedSource, std::vector<Warning>& warnings);
        void checkCallExpression(TSNode node, const ParsedSource& parsedSource, std::vector<Warning>& warnings);
        std::unordered_set<std::string> checkBinaryExpression(TSNode node, const ParsedSource& parsedSource, std::vector<Warning>& warnings);
        void checkForDataClumps(std::vector<Warning>& warnings);
        void storeClumpInfo(std::vector<int>& currentLines, std::unordered_set<std::string>& variablesInScope);
        void createBitsets();
        void expandNode(const std::vector<std::string>& variables, std::vector<std::string>& candidates, int index);
        bool connectsToAll(const std::vector<std::string>& group, const std::string& candidate);
        bool isSubset(const std::vector<std::string>& subset, const std::vector<std::string>& superset);
        void removeSubsetsFromClumps();
    public:
        DataClumpDetector() = default;
        std::vector<Warning> analyzeSource(const ParsedSource& parsedSource) override;
};

#endif // DATA_CLUMP_DETECTOR_H