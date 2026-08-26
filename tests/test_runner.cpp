#include "SmellyCodeDetector.h"

#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

struct TestCase {
    std::string name;
    std::string suite;
    std::string fixture;
    std::string expectedCategory;
    bool expectCategory;
};

static const std::vector<TestCase> testCases = {
    {"clean-control", "smoke", "tests/fixtures/clean_test.cpp", "", false},
    {"repeated-code", "smoke", "tests/fixtures/repeatedTest.cpp", "repeated-code", true},
    {"deep-if", "smoke", "tests/fixtures/ifStatementTest.cpp", "deep-if", true},
    {"dead-function", "smoke", "tests/fixtures/deadCodeTest.cpp", "unused-function", true},
    {"throw-exit-limit", "edge", "tests/edge_cases/throw_exit.cpp", "unreachable-code", false},
};

static bool hasCategory(const std::vector<Warning>& warnings, const std::string& category) {
    for (const Warning& warning : warnings) {
        if (warning.category == category) return true;
    }
    return false;
}

static void printUsage() {
    std::cout << "Usage: project4_tests [--suite smoke|edge|all] [--case NAME]\n";
}

int main(int argc, char** argv) {
    std::string selectedSuite = "all";
    std::string selectedCase;

    for (int i = 1; i < argc; ++i) {
        std::string argument = argv[i];
        if (argument == "--suite" && i + 1 < argc) {
            selectedSuite = argv[++i];
        } else if (argument == "--case" && i + 1 < argc) {
            selectedCase = argv[++i];
        } else if (argument == "--help") {
            printUsage();
            return 0;
        } else {
            printUsage();
            return 2;
        }
    }

    int selectedCount = 0;
    int failedCount = 0;
    for (const TestCase& testCase : testCases) {
        if (selectedSuite != "all" && testCase.suite != selectedSuite) continue;
        if (!selectedCase.empty() && testCase.name != selectedCase) continue;
        ++selectedCount;

        std::ifstream inputFile(testCase.fixture);
        if (!inputFile.is_open()) {
            std::cout << "[FAIL] " << testCase.name << ": could not open " << testCase.fixture << "\n";
            ++failedCount;
            continue;
        }

        SmellyCodeDetector detector(inputFile);
        std::ostringstream detectorOutput;
        std::streambuf* originalOutput = std::cout.rdbuf(detectorOutput.rdbuf());
        detector.runDetectors();
        std::cout.rdbuf(originalOutput);
        const bool categoryFound = testCase.expectedCategory.empty()
            ? !detector.getWarnings().empty()
            : hasCategory(detector.getWarnings(), testCase.expectedCategory);
        const bool passed = categoryFound == testCase.expectCategory;

        std::cout << (passed ? "[PASS] " : "[FAIL] ") << testCase.name;
        if (!testCase.expectedCategory.empty()) {
            std::cout << " expected " << (testCase.expectCategory ? "presence of " : "absence of ")
                      << testCase.expectedCategory;
        } else {
            std::cout << " expected a clean report";
        }
        std::cout << "; observed " << detector.getWarnings().size() << " warning(s)\n";
        if (!passed) ++failedCount;
    }

    if (selectedCount == 0) {
        std::cout << "No test cases matched the requested parameters.\n";
        return 2;
    }

    std::cout << "Suite result: " << selectedCount - failedCount << "/" << selectedCount
              << " passed\n";
    return failedCount == 0 ? 0 : 1;
}
