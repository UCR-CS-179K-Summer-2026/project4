#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <regex>
#include <vector>
#include <set>
#include <algorithm>

struct DeclaredVariable {
    std::string name;
    int line;
};

// Keywords we recognize as the start of a variable declaration. Extend this list as needed.
const std::vector<std::string> TYPE_KEYWORDS = {
    "int", "float", "double", "char", "bool", "long", "short",
    "unsigned", "auto", "std::string", "string"
};

std::string buildTypePattern() {
    std::ostringstream oss;
    oss << "(?:";
    for (size_t i = 0; i < TYPE_KEYWORDS.size(); ++i) {
        if (i > 0) oss << "|";
        
        std::string t = TYPE_KEYWORDS[i];
        std::string escaped;
        for (char c : t) {
            if (c == '.') escaped += "\\.";
            else escaped += c;
        }
        oss << escaped;
    }
    oss << ")";
    return oss.str();
}

// Strips single-line (//) and block (/* */) comments from the source
std::string stripComments(const std::string& source) {
    std::string noBlockComments = std::regex_replace(
        source, std::regex(R"(/\*[\s\S]*?\*/)"), "");
    std::string noLineComments = std::regex_replace(
        noBlockComments, std::regex(R"(//.*)"), "");
    return noLineComments;
}

// Finds variable declarations of the form:
//   <type> name = ...;
//   <type> name;
//   <type> a, b, c;
std::vector<DeclaredVariable> findDeclarations(const std::string& source) {
    std::vector<DeclaredVariable> declarations;

    static const std::string typePattern = buildTypePattern();
    static const std::regex declRegex(
        typePattern + R"(\s+([a-zA-Z_][a-zA-Z0-9_ ,=\-\+\*\/\.\(\)]*);)"
    );

    std::istringstream stream(source);
    std::string line;
    int lineNum = 0;

    while (std::getline(stream, line)) {
        ++lineNum;
        auto begin = std::sregex_iterator(line.begin(), line.end(), declRegex);
        auto end = std::sregex_iterator();

        for (auto it = begin; it != end; ++it) {
            std::string declaratorList = (*it)[1].str();

            // Split declaratorList on commas to support "int a, b = 2;"
            std::regex declaratorRegex(R"(([a-zA-Z_][a-zA-Z0-9_]*)\s*(?:=[^,]*)?)");
            auto dBegin = std::sregex_iterator(
                declaratorList.begin(), declaratorList.end(), declaratorRegex);
            auto dEnd = std::sregex_iterator();

            for (auto dIt = dBegin; dIt != dEnd; ++dIt) {
                std::string name = (*dIt)[1].str();
                declarations.push_back({name, lineNum});
            }
        }
    }

    return declarations;
}

// Counts whole-word occurrences of `name` in `source`.
int countUsages(const std::string& source, const std::string& name) {
    std::regex wordRegex("\\b" + name + "\\b");
    auto begin = std::sregex_iterator(source.begin(), source.end(), wordRegex);
    auto end = std::sregex_iterator();
    return static_cast<int>(std::distance(begin, end));
}

// Returns pairs of (functionBodyText, startLineOfBody).
std::vector<std::pair<std::string, int>> splitIntoFunctionBodies(
    const std::string& source) {

    std::vector<std::pair<std::string, int>> bodies;

    static const std::regex funcStartRegex(
        R"([a-zA-Z_][a-zA-Z0-9_:<>]*\s+[a-zA-Z_][a-zA-Z0-9_]*\s*\([^;{}]*\)\s*\{)"
    );

    auto begin = std::sregex_iterator(source.begin(), source.end(), funcStartRegex);
    auto end = std::sregex_iterator();

    for (auto it = begin; it != end; ++it) {
        size_t openBracePos = it->position() + it->length() - 1; // the "{"
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

int main(int argc, char* argv[]) {
    if (argc != 2) {
        std::cerr << "Usage: " << argv[0] << " <path_to_cpp_file>\n";
        return 1;
    }

    std::ifstream file(argv[1]);
    if (!file) {
        std::cerr << "Error: could not open file " << argv[1] << "\n";
        return 1;
    }

    std::stringstream buffer;
    buffer << file.rdbuf();
    std::string rawSource = buffer.str();
    std::string source = stripComments(rawSource);

    auto functionBodies = splitIntoFunctionBodies(source);

    bool foundAny = false;
    for (const auto& fb : functionBodies) {
        const std::string& body = fb.first;
        int startLine = fb.second;
        std::vector<DeclaredVariable> declarations = findDeclarations(body);

        for (const auto& decl : declarations) {
            int occurrences = countUsages(body, decl.name);
            // 1 occurrence = only the declaration itself -> unused
            if (occurrences <= 1) {
                int actualLine = startLine + decl.line - 1;
                std::cout << "Warning: Redundant dead/unused code. \""
                        << decl.name << "\" is declared but never used. "
                        << "(line " << actualLine << ")\n";
                foundAny = true;
            }
        }
    }

    if (!foundAny) {
        std::cout << "No dead/unused variables found.\n";
    }

    return 0;
}