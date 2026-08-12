#include "NameAnalyzer.h"
#include <iostream>
#include <regex>

std::string NameAnalyzer::extractIdentifierName(const ParsedSource& parsedSource, TSNode identifierNode) {
    uint32_t startByte = ts_node_start_byte(identifierNode);
    uint32_t endByte = ts_node_end_byte(identifierNode);
    return parsedSource.source.substr(startByte, endByte - startByte);
}

int NameAnalyzer::getLineNumber(const ParsedSource& parsedSource, TSNode node) {
    uint32_t startByte = ts_node_start_byte(node);
    return std::count(parsedSource.source.begin(), parsedSource.source.begin() + startByte, '\n') + 1;
}

TSNode NameAnalyzer::findIdentifierNode(TSNode node) {
    if(ts_node_is_null(node)) {
        return node;
    }

    if(strcmp(ts_node_type(node), "identifier") == 0) {
        return node;
    }

    uint32_t childCount = ts_node_child_count(node);
    for(uint32_t i = 0; i < childCount; ++i) {
        TSNode child = ts_node_child(node, i);
        TSNode result = findIdentifierNode(child);

        if(!ts_node_is_null(result)) {
            return result;
        }
    }

    return {};
}

bool NameAnalyzer::isPoorName(const std::string& name, const std::string& type) {
    if(name.length() < 3 && name != "id" && name != "i" && name != "j" && name != "k") {
        return true;
    }

    static std::regex boolPrefixPattern(R"(\b(is|has|can|should|was|will)[A-Z].*)");
    if(type == "bool" && !std::regex_match(name, boolPrefixPattern)) {
        return true;
    }

    static std::regex functionNamePattern(R"(\b(func|function|do|perform|execute|calc|calculate|test|run|call|process|init|initialize|handle)\b)");
    if(type == "function" && std::regex_match(name, functionNamePattern)) {
        return true;
    }

    static std::regex poorNamePattern(R"(\b(temp|data|str|tmp|var|test|flag|item|items|val|value|values|element|elements|arr|array|list|vector|vec)\b)");
    if(std::regex_match(name, poorNamePattern)) {
        return true;
    }

    return false;
}

void NameAnalyzer::outputErrorMessage(const std::string& name, const int& line, std::vector<Warning>& warnings) {
    warnings.push_back({
        line,
        "Poor Naming",
        "Poor identifier name detected: '" + name + "'. Consider using a more descriptive name."
    });
}