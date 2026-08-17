#include "SmellyCodeDetector.h"
#include "Parser.h"
#include <algorithm>
#include <iostream>
#include <tree_sitter/api.h>

SmellyCodeDetector::SmellyCodeDetector(const std::ifstream& inputFile) {
    parsedSource = parser.parse(const_cast<std::ifstream&>(inputFile));

    detectors.push_back(&poorNameChecker);
    detectors.push_back(&redundantCodeChecker);
    detectors.push_back(&deadCodeChecker);
    detectors.push_back(&repeatedCodeChecker);
    detectors.push_back(&commentChecker);
    detectors.push_back(&deepIfDetector);
    detectors.push_back(&dataClumpDetector);
    detectors.push_back(&memoryChecker);
    detectors.push_back(&longParamListDetector);
    detectors.push_back(&inheritanceChecker);
}

int SmellyCodeDetector::runDetectors() {
    std::vector<Warning> allWarnings;

    for (Detector* detector : detectors) {
        std::vector<Warning> found = detector->analyzeSource(parsedSource);
        allWarnings.insert(allWarnings.end(), found.begin(), found.end());
    }

    std::sort(allWarnings.begin(), allWarnings.end(),
        [](const Warning& a, const Warning& b) {
            if (a.line != b.line) return a.line < b.line;
            return a.category < b.category; // tie-break: which check found it
        });

    for (const auto& w : allWarnings) {
        std::cout << "Warning: [" << w.category << "]. " << w.message << "(line " << w.line <<")" <<  "\n";
    }

    int totalWarnings = static_cast<int>(allWarnings.size());

    ts_tree_delete(parsedSource.tree);
    return totalWarnings;
}