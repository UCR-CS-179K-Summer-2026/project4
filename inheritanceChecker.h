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

        void scanForInheritance(TSNode classDefNode, const std::string& source, const std::vector<std::string>& baseNames, std::vector<bool> baseUsed, bool& overrideFound) const;

        void checkClass(TSNode classNode, const ParsedSource& parsedSource, std::vector<Warning>& warnIngs) const;

        TSNode findClassByName(TSNode node, const std::string& name, const std::string& source) const;

        bool isBaseUsedExternally(TSNode root, const ParsedSource& parsedSource, const std::string& derivedClassName, TSNode derivedBodyNode, const std::string& baseName) const;
        
        void collectInstanceOfType(TSNode node, const std::string& source, const std::string& typeName, std::vector<std::string>& instanceNames) const;

        std::vector<std::string> collectMemberNames(TSNode classBodyNode, const std::string& source) const;

        void checkExternalMemberUsage(TSNode node, const std::string& source, const std::vector<std::string>& instanceNames, const std::vector<std::string>& memberNames, bool& found) const;

        TSNode findFunctionByName(TSNode node, const std::string& name, const std::string& source) const;
        void scanForSlicingUsage(TSNode node, TSNode root, const std::string& source, const std::string& baseName, const std::vector<std::string>& derivedInstanceNames,const std::vector<std::string>& baseInstanceNames, bool& found) const; 

        bool isBaseUsedViaSlicing(TSNode root, const ParsedSource& parsedSource, const std::string& derivedClassName, const std::string& baseName) const;

};

#endif