#include "SmellyCodeDetector.h"
#include "Parser.h"
#include <algorithm>
#include <iostream>
#include <tree_sitter/api.h>

SmellyCodeDetector::SmellyCodeDetector(const std::ifstream& inputFile) {
    parsedSource = parser.parse(const_cast<std::ifstream&>(inputFile));

    detectors.push_back(&poorNameChecker);
    detectors.push_back(&redundantCodeChecker);
    detectors.push_back(&repeatedCodeChecker);
    detectors.push_back(&commentChecker);
    detectors.push_back(&deepIfDetector);
}

int SmellyCodeDetector::runDetectors() {
    int totalWarnings = 0;
    totalWarnings += poorNameChecker.analyzeSource(parsedSource);
    totalWarnings += redundantCodeChecker.analyzeSource(parsedSource);
    totalWarnings += repeatedCodeChecker.analyzeSource(parsedSource);
    totalWarnings += commentChecker.analyzeSource(parsedSource);
    totalWarnings += deepIfDetector.analyzeSource(parsedSource);
    totalWarnings += longParamListDetector.analyzeSource(parsedSource);

    ts_tree_delete(parsedSource.tree);
    return totalWarnings;
}