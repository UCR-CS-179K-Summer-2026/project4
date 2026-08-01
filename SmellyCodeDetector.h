#ifndef SMELLY_CODE_DETECTOR_H
#define SMELLY_CODE_DETECTOR_H

#include <fstream>
#include "ParsedSource.h"
#include "Parser.h"
#include "poorNameChecker.h"
#include "redundantCodeChecker.h"

class SmellyCodeDetector {
    private:
        PoorNameChecker poorNameChecker;
        RedundantCodeChecker redundantCodeChecker;
        Parser parser;
        ParsedSource parsedSource;
    public:
        SmellyCodeDetector(const std::ifstream& inputFile);
        int runDetectors();
};

#endif