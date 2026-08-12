#ifndef SMELLY_CODE_DETECTOR_H
#define SMELLY_CODE_DETECTOR_H

#include <fstream>
#include <vector>
#include "ParsedSource.h"
#include "Parser.h"
#include "Detector.h"
#include "poorNameChecker.h"
#include "redundantCodeChecker.h"
#include "commentChecker.h"
#include "repeatedCodeChecker.h"
#include "DeepIfDetector.h"
#include "longParamList.h"

class SmellyCodeDetector {
    private:
        PoorNameChecker poorNameChecker;
        RedundantCodeChecker redundantCodeChecker;
        commentChecker commentChecker;
        RepeatedCodeChecker repeatedCodeChecker;
        DeepIfDetector deepIfDetector;
        longParamList longParamListDetector;

        std::vector<Detector*> detectors;

        Parser parser;
        ParsedSource parsedSource;
    public:
        SmellyCodeDetector(const std::ifstream& inputFile);
        int runDetectors();
};

#endif