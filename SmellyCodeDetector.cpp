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
    std::vector<Warning> foundWarnings;   // renamed to avoid shadowing the member

    for (Detector* detector : detectors) {
        std::vector<Warning> found = detector->analyzeSource(parsedSource);
        foundWarnings.insert(foundWarnings.end(), found.begin(), found.end());
    }

    std::sort(foundWarnings.begin(), foundWarnings.end(),
        [](const Warning& a, const Warning& b) {
            if (a.line != b.line) return a.line < b.line;
            return a.category < b.category;
        });

    for (const auto& w : foundWarnings) {
        std::cout << "Warning: [" << w.category << "]. " << w.message << "(line " << w.line << ")" << "\n";
    }

    allWarnings = foundWarnings;   // now actually populates the member

    int totalWarnings = static_cast<int>(allWarnings.size());

    ts_tree_delete(parsedSource.tree);
    return totalWarnings;
}