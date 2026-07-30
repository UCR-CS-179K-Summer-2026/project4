#include "redundantCodeChecker.h"

#include <iostream>
#include <sstream>
#include <string>
#include <regex>
#include <vector>
#include <algorithm>

namespace {

    struct DeclaredVariable {
        std::string name;
        int line;
    };

    // key words recognized as variable declarations
    const std::vector<std::string> TYPE_KEYWORDS = {
        "int", "float", "double", "char", "bool", "long", "short", "unsigned", "auto", "std::string", "string"
    };

    // builds the regex pattern used by findDeclarations() to recognize supported type keywords
    std::string buildTypePattern() {
        std::ostringstream oss;
        oss << "(?:";
        for (size_t i = 0; i < TYPE_KEYWORDS.size(); ++i) {
            if(i > 0) oss << "|";
            std::string escaped;
            for (char c: TYPE_KEYWORDS[i]) {
                if(c== '.') escaped += "\\.";
                else escaped += c;
            }
            oss << escaped;
        }
        oss << ")";
        return oss.str();
    }

    // stripes single line (//) and block (/* */) comments
    std::string stripComments(const std::string& source) {
        std::string noBlockComments = std::regex_replace(source, std::regex(R"(/\*[\s\S]*?|*/)"),"");
        std::string noLineComments = std::regex_replace(noBlockComments, std::regex(R"(//.*)"), "");
        return noLineComments;
    }

    // finds variable declaration statements within a function body and extracts each declared variable's name and line number
    // (e.g. int x;, int x = 5;, int a, b;)
    std::vector<DeclaredVariable> findDeclarations(const std::string& body) {
        std::vector<DeclaredVariable> declarations;

        static const std::string typePattern = buildTypePattern();
        static const std::regex declRegex(typePattern + R"(\s+([a-zA-Z_][a-zA-Z0-9_ ,=\-\+\*\/\.\(\)]*);)");
        static const std::regex declaratorRegex(R"(([a-zA-Z_][a-zA-Z0-9_]*)\s*(?:=[^,]*)?)");

        std::istringstream stream(body);
        std::string line;
        int lineNum = 0;

        while (std::getline(stream, line)) {
            ++lineNum;
            auto begin = std::sregex_iterator(line.begin(), line.end(), declRegex);
            auto end = std::sregex_iterator();

            for (auto it = begin; it!= end; ++it) {
                std::string declaratorList = (*it)[1].str();

                auto dBegin = std::sregex_iterator(
                    declaratorList.begin(), declaratorList.end(), declaratorRegex);
                auto dEnd = std::sregex_iterator();

                for (auto dIt = dBegin; dIt != dEnd; ++dIt) {
                    declarations.push_back({(*dIt)[1].str(), lineNum});
                }
            }
        }
        return declarations;
    }

    // counts amount of times a given variable name appears as a whole word within a function body
    // determines whether a declared variable is referenced again
    int countUsages(const std::string& source, const std::string& name) {
        std::regex wordRegex("\\b" + name + "\\b");
        auto begin = std::sregex_iterator(source.begin(), source.end(), wordRegex);
        auto end = std::sregex_iterator();
        return static_cast<int>(std::distance(begin, end));
    }

    std::vector<std::pair<std::string, int>> splitIntoFunctionBodies(const std::string& source) {
        std::vector<std::pair<std::string, int>> bodies;
        
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
                bodies.push_back({body, startLine});
            }
        }

        return bodies;
    }

} // end of namespace

int redundantCodeChecker(std::ifstream& inputFile) {
    std::stringstream buffer;
    buffer << inputFile.rdbuf();
    std::string rawSource = buffer.str();
    std::string source = stripComments(rawSource);

    inputFile.clear();
    inputFile.seekg(0, std::ios::beg);

    auto functionBodies = splitIntoFunctionBodies(source);

    int warningCount = 0;
    for (const auto& fb : functionBodies) {
        const std::string& body = fb.first;
        int startLine = fb.second;
        std::vector<DeclaredVariable> declarations = findDeclarations(body);

        for (const auto& decl : declarations) {
            int occurrences = countUsages(body, decl.name);
            if (occurrences <= 1) {
                int actualLine = startLine + decl.line - 1;
                std::cout << "Warning: Redundant dead/unused code. \""
                        << decl.name << "\" is declared but never used. "
                        << "(line " << actualLine << ")\n";
                ++warningCount;
            }
        }
    }

    return warningCount;
}