#ifndef DETECTOR_H
#define DETECTOR_H

#include "ParsedSource.h"
#include <tree_sitter/api.h>
#include <vector>
#include <string>
#include <optional>
#include <cstdint>

struct Edit {
    uint32_t startByte;
    uint32_t endByte;
    std::string replacement;
};

struct Warning {
    int line;
    std::string category;   // which check found it, e.g. "dead-variable", "redundant-boolean", "chained-if"
    std::string message;
    std::optional<Edit> fix{};
};

// Declared here, defined in FixApplier.cpp (see below)
std::string applyFixes(const std::string& source, const std::vector<Warning>& warnings);
class Detector {
    private:
        virtual void visitNode(TSNode node, const ParsedSource& parsedSource, std::vector<Warning>& warnings) = 0;
    public:
        virtual std::vector<Warning> analyzeSource(const ParsedSource& parsedSource) = 0;
        virtual ~Detector() = default;
};

#endif