#ifndef LONG_PARAM_LIST_H
#define LONG_PARAM_LIST_H

#include <string>
#include "ParsedSource.h"
#include "Detector.h"

class longParamList: public Detector{

public:
    longParamList() = default;
    std::vector<Warning> analyzeSource(const ParsedSource& source) override;

private:
    virtual void visitNode(TSNode node, const ParsedSource& parsedSource, std::vector<Warning>& warnings) override;

    static constexpr int kMaxParams = 4;

    TSNode findIDNode(TSNode node) const;
    std::string extractFunctionName(TSNode functionDefNode, const std::string& source) const;

    void reportLongParamList(const std::string& functionName, int paramCount, int line, std::vector<Warning>& warnings) const;
};
#endif 