#ifndef SMELLY_CODE_DETECTOR_H
#define SMELLY_CODE_DETECTOR_H

#include <fstream>
#include <vector>
#include "ParsedSource.h"
#include "Parser.h"
#include "Detector.h"
#include "poorNameChecker.h"
#include "redundantCodeChecker/redundantCodeChecker.h"
#include "commentChecker/commentChecker.h"
#include "repeatedCodeChecker.h"
#include "DeepIfDetector.h"
#include "DataClumpDetector.h"
#include "memoryChecker/memoryChecker.h"
#include "longParamList.h"
#include "deadCodeChecker/deadCodeChecker.h"
#include "inheritanceChecker.h"

class SmellyCodeDetector {
    private:
        PoorNameChecker poorNameChecker;
        RedundantCodeChecker redundantCodeChecker;
        commentChecker commentChecker;
        RepeatedCodeChecker repeatedCodeChecker;
        DeepIfDetector deepIfDetector;
        DataClumpDetector dataClumpDetector;
        memoryChecker memoryChecker;
        longParamList longParamListDetector;
        DeadCodeChecker deadCodeChecker;
        InheritanceChecker inheritanceChecker;

        std::vector<Detector*> detectors;

        Parser parser;
        ParsedSource parsedSource;
        std::vector<Warning> allWarnings;
    public:
        SmellyCodeDetector(const std::ifstream& inputFile);
        int runDetectors();

        // accessors so main.cpp can apply fixes after the fact
        const std::vector<Warning>& getWarnings() const { return allWarnings; }
        const ParsedSource& getParsedSource() const { return parsedSource; }
};

#endif