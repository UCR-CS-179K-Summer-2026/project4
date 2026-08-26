#ifndef FUNCTION_INFO_H
#define FUNCTION_INFO_H

#include <string>
#include <vector>
#include "DeclaredVariable.h"

// Represents information about a function, including its starting line, body, declaration, and declared variables
struct FunctionInfo {
    std::string functionStart;
    std::string functionBody;
    int line;
    std::vector<DeclaredVariable> variables;
};

#endif