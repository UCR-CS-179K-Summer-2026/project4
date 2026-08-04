#include "SmellyCodeDetector.h"
#include "Parser.h"
#include <iostream>
#include <tree_sitter/api.h>

SmellyCodeDetector::SmellyCodeDetector(const std::ifstream& inputFile) {
    parsedSource = parser.parse(const_cast<std::ifstream&>(inputFile));
}

int SmellyCodeDetector::runDetectors() {
    int totalWarnings = 0;
    totalWarnings += poorNameChecker.analyzeSource(parsedSource);
    totalWarnings += redundantCodeChecker.analyzeSource(parsedSource);
    totalWarnings += repeatedCodeChecker.analyzeSource(parsedSource);
    totalWarnings += commentChecker.analyzeSource(parsedSource);

    ts_tree_delete(parsedSource.tree);
    return totalWarnings;
}