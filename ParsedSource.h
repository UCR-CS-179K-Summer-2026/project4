#ifndef PARSED_SOURCE_H
#define PARSED_SOURCE_H

#include <string>
#include <vector>
#include <tree_sitter/api.h>

// Includes the raw source code (functions, comments, etc.) and a vector of FunctionInfo
// objects that represents the parsed functions in the source code
struct ParsedSource {
    std::string source;
    TSTree *tree;
};

#endif