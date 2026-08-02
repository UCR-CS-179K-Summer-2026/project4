#ifndef REDUNDANT_CODE_CHECKER_H
#define REDUNDANT_CODE_CHECKER_H

#include "ParsedSource.h"
#include "Detector.h"

// Detects declared-but-unused local variables within each parsed
// function (redundant/dead code).
class RedundantCodeChecker : public Detector {
    private:
        int countUsages(const std::string& body, const std::string& name);
    public:
        RedundantCodeChecker() = default;
        int analyzeSource(const ParsedSource& parsedSource) override;
};

#endif