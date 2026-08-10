#include <sstream>
#include <regex>
#include <tree_sitter/api.h>
#include <iostream>
#include "ParsedSource.h"
#include "Parser.h"

extern "C" {
    const TSLanguage *tree_sitter_cpp(void);
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

    // ts_tree_delete(tree);
    ts_parser_delete(parser);

    return {rawSource, tree};
}