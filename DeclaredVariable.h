#ifndef DECLARED_VARIABLE_H
#define DECLARED_VARIABLE_H

#include <string>
#include <vector>

// Represents a declared variable with its name and the line number where it was declared
struct DeclaredVariable {
    std::string name;
    int line;
};

#endif