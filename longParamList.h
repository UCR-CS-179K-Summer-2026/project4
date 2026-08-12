#ifndef LONG_PARAM_LIST_H
#define LONG_PARAM_LIST_H

#include <string>
#include "ParsedSource.h"
#include "Detector.h"

class longParamList: public Detector{

public:
    longParamList() = default;
    int analyzeSource(const ParsedSource& source) override;

private:
    void visitNode(TSNode node, const ParsedSource& parsedSource, int& warningCount) override;

    static constexpr int kMaxParams = 4;

    TSNode findIDNode(TSNode node) const;
    std::string extractFunctionName(TSNode functionDefNode, const std::string& source) const;

    void reportLongParamList(const std::string& functionName, int paramCount, int line ) const;
};
#endif 