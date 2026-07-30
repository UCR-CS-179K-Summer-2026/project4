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
    std::string stipComments(const std::string& source) {
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

}