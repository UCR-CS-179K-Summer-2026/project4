#ifndef REDUNDANT_CODE_CHECKER_H
#define REDUNDANT_CODE_CHECKER_H

#include <fstream>
#include "ParsedSource.h"
#include "Detector.h"

class RedundantCodeChecker : public Detector {
    private:
        int countUsages(const std::string& body, const std::string& variableName);
    public:
        RedundantCodeChecker() = default;
        int analyzeSource(const ParsedSource& parsedSource) override;
};

#endif