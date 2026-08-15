#ifndef INHERITANCE_CHECKER_H
#define INHERITANCE_CHECKER_H

#include <string>
#include <vector>
#include "ParsedSource.h"
#include "Detector.h"

class InheritanceChecker : public Detector {
    public:
        InheritanceChecker() = default;
        std::vector<Warning> analyzeSource(const ParsedSource& parsedSource) override;
    
    private:
        void visitNode(TSNode node, const ParsedSource& parsedSource, std::vector<Warning>& warnings) override;
        std::string nodeText(TSNode node, const std::string& source) const;

        TSNode findChildByType(TSNode node, const char* type) const;

        std::vector<std::string> extractBaseClasses(TSNode classDefNode, const std::string& source) const;

        void scanForInheritance(TSNode classDefNode, const std::string& source, std::vector<bool> baseUsed, std::vector<Warning>& warnings) const;

        void checkClass(TSNode classNode, const ParsedSource& parsedSource, std::vector<Warning>& warnIngs) const;

};

#endif