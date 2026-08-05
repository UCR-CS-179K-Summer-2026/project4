#include "Parser.h"
#include <sstream>
#include <regex>
#include <tree_sitter/api.h>
#include <iostream>
extern "C" {
    const TSLanguage *tree_sitter_cpp(void);
}

std::string Parser::buildTypePattern() {
    const std::vector<std::string> TYPE_KEYWORDS = {
        "int", "float", "double", "char", "bool", "long", "short",
        "unsigned", "auto", "std::string", "string"
    };

    std::ostringstream oss;
    oss << "(?:";
    for (size_t i = 0; i < TYPE_KEYWORDS.size(); ++i) {
        if (i > 0) oss << "|";
        std::string escaped;
        for (char c : TYPE_KEYWORDS[i]) {
            if (c == '.') escaped += "\\.";
            else escaped += c;
        }
        oss << escaped;
    }
    oss << ")";
    return oss.str();
}

std::string Parser::stripComments(const std::string& source) {
    std::string noBlockComments = std::regex_replace(
        source, std::regex(R"(/\*[\s\S]*?\*/)"), "");
    std::string noLineComments = std::regex_replace(
        noBlockComments, std::regex(R"(//.*)"), "");
    return noLineComments;
}

std::string Parser::stripStrings(const std::string& source) {//strip string literals from source
    std::string noStrings = std::regex_replace(source, std::regex(R"("([^"\\]|\\.)*")"), "");//replace string keys with blank space
    return noStrings;
}

std::vector<DeclaredVariable> Parser::findDeclarations(const std::string& body) {
    std::vector<DeclaredVariable> declarations;

    static const std::string typePattern = buildTypePattern();
    static const std::regex declRegex(
        typePattern + R"(\s+([a-zA-Z_][a-zA-Z0-9_ ,=+*/.()-]*);)"
    );
    static const std::regex declaratorRegex(
        R"(([a-zA-Z_][a-zA-Z0-9_]*)\s*(?:=[^,]*)?)"
    );

    std::istringstream stream(body);
    std::string line;
    int lineNumber = 0;

    while (std::getline(stream, line)) {
        lineNumber++;
        std::smatch match;
        if (std::regex_search(line, match, declRegex)) {
            std::string declarators = match[1].str();
            auto declaratorBegin = std::sregex_iterator(declarators.begin(), declarators.end(), declaratorRegex);
            auto declaratorEnd = std::sregex_iterator();

            for (auto it = declaratorBegin; it != declaratorEnd; ++it) {
                declarations.push_back({(*it)[1].str(), lineNumber});
            }
        }
    }

    return declarations;
}

std::vector<FunctionInfo> Parser::splitIntoFunctionBodies(const std::string& source) {
    std::vector<FunctionInfo> functions;

    static const std::regex funcStartRegex(
        R"([a-zA-Z_][a-zA-Z0-9_:<>]*\s+[a-zA-Z_][a-zA-Z0-9_]*\s*\([^;{}]*\)\s*\{)"
    );

    auto begin = std::sregex_iterator(source.begin(), source.end(), funcStartRegex);
    auto end = std::sregex_iterator();

    for (auto it = begin; it != end; ++it) {
        size_t openBracePos = it->position() + it->length() - 1;
        int depth = 1;
        size_t pos = openBracePos + 1;
        size_t bodyStart = pos;

        while (pos < source.size() && depth > 0) {
            if (source[pos] == '{') depth++;
            else if (source[pos] == '}') depth--;
            pos++;
        }

        if (depth == 0) {
            std::string body = source.substr(bodyStart, pos - bodyStart - 1);
            int startLine = static_cast<int>(
                std::count(source.begin(), source.begin() + bodyStart, '\n')) + 1;
            functions.push_back({it->str(), body, startLine});
        }
    }

    return functions;
}

ParsedSource Parser::parse(std::ifstream& inputFile) {
    TSParser *parser = ts_parser_new();

    ts_parser_set_language(parser, tree_sitter_cpp());
    std::stringstream buffer;
    buffer << inputFile.rdbuf();
    std::string rawSource = buffer.str();

    TSTree *tree = ts_parser_parse_string(parser, nullptr, rawSource.c_str(), rawSource.size());
    if(tree == nullptr) {
        std::cerr << "Error: Failed to parse the source code." << std::endl;
        ts_parser_delete(parser);
        return {rawSource, nullptr};
    }
    TSNode rootNode = ts_tree_root_node(tree);
    char* treeString = ts_node_string(rootNode);
    std::cout << "Parse tree:\n" << treeString << std::endl;
    free(treeString);  // Free the string returned by ts_node_string

    // ts_tree_delete(tree);
    ts_parser_delete(parser);

    std::string source = stripComments(rawSource);
    // std::vector<FunctionInfo> functions = splitIntoFunctionBodies(source);
    // for (auto& func : functions) {
    //     func.variables = findDeclarations(func.functionBody);
    // }

    return {rawSource, tree};
}