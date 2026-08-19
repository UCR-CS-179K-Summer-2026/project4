#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <vector>
#include <algorithm>

#include "SmellyCodeDetector.h"

int main() {
    int totalSmellyCount = 0;
    int processedFiles = 0;
    std::string line;

    std::cout << "Enter the name(s) of the file(s) to read: ";
    std::getline(std::cin, line);

    std::istringstream lineStream(line);
    std::vector<std::string> fileNames;
    std::string fileName;

    while (lineStream >> fileName) {
        fileNames.push_back(fileName);
    }

    if (fileNames.empty()) {
        std::cout << "No file names provided." << std::endl;
        return 1;
    }

    for (const auto& name : fileNames) {
        std::ifstream inputFile(name);

        if (!inputFile.is_open()) {
            std::cout << "Error opening file: " << name << std::endl;
            continue;
        }

        std::cout << "Analyzing file: " << name << std::endl;

        SmellyCodeDetector detector(inputFile);
        int smellyCount = detector.runDetectors();

        if (smellyCount > 0) {
            std::cout << smellyCount << " Smelly code(s) found in the file: " << name << std::endl;
        } else {
            std::cout << "No smelly code found in file: " << name << std::endl;
        }
        std::cout << std::endl;

        totalSmellyCount += smellyCount;
        ++processedFiles;
        inputFile.close();

        // --- Fix prompt, per file, using this file's own warnings/source ---
        const std::vector<Warning>& warnings = detector.getWarnings();
        bool anyFixable = std::any_of(warnings.begin(), warnings.end(),
            [](const Warning& w) { return w.fix.has_value(); });

        if (anyFixable) {
            std::cout << "Some detected smells in " << name
                        << " have automatic fixes available. Apply them? (y/n): ";
            char response;
            std::cin >> response;
            std::cin.ignore(); // clear trailing newline before any later getline

            if (response == 'y' || response == 'Y') {
                std::vector<Warning> warningsCopy = warnings; // applyFixes takes non-const ref
                std::string fixedSource = applyFixes(detector.getParsedSource().source, warnings);

                std::string outputPath = name + ".fixed.cpp";
                std::ofstream out(outputPath);
                out << fixedSource;
                out.close();

                std::cout << "Fixed file written to: " << outputPath << "\n";
            }
            std::cout << std::endl;
        }
    }

    if (processedFiles == 0) {
        std::cout << "No files were processed." << std::endl;
        return 1;
    }

    std::cout << "Total smelly code(s) found across all files: " << totalSmellyCount << std::endl;
    std::cout << "Total files processed: " << processedFiles << std::endl;

    return 0;
}