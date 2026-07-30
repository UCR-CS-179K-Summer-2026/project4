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

    std::string buildTypePattern() {
        std::ostringstream oss;
        oss << "(>:";
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
}