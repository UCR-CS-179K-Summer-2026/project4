#include "commentChecker.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <regex>
#include <string>
#include <vector>
#include <tree_sitter/api.h>

extern "C" {
    const TSLanguage *tree_sitter_cpp();
}

//scans through the file for functions, detecting preceding comments, and then calls helper function to scan those functions for interior or inline comments
void commentChecker::visitNode(TSNode node, const ParsedSource& parsedSource, std::vector<Warning>& warnings) {
    int functionLine;//for outputting later
    bool hasComment = false;//flag for if the function has a comment

    //convert to string view for comparison
    std::string_view nodeType(ts_node_type(node));
    
    //check if the node is a function definition
    if(nodeType == "function_definition"){
        TSPoint funcStart = ts_node_start_point(node);//location of function start
        TSPoint funcEnd = ts_node_end_point(node);//location of function end
        functionLine = funcStart.row + 1;//save function start row to function line, plus one because default line indexing starts from 0
        
        hasComment = false;//set flag for this function

        //check the node above for a preceding comment
        TSNode prevNode = ts_node_prev_sibling(node);
        if(prevNode.id != nullptr && std::string_view(ts_node_type(prevNode)) == "comment"){
            hasComment = true;//if comment found, set flag to true
        }

        //check for inline or internal comments
        if (!hasComment) {//if we did not already find a comment
            hasComment = scanForComments(node);//scan for comments with helper function
        }

        //check for comments on the last line of the function, immediately after the closing bracket
        if (!hasComment) {
            //check if the next node is a comment
            TSNode nextNode = ts_node_next_sibling(node);
            if (nextNode.id != nullptr && std::string_view(ts_node_type(nextNode)) == "comment") {
                TSPoint commentStart = ts_node_start_point(nextNode);
                
                // if the next node is a comment, check if it's on the same line as the closing bracket
                if (commentStart.row == funcEnd.row) {
                    hasComment = true;//if so, set flag to true
                }
            }
        }

        //warning if no comment found
        if (!hasComment) {
            warnings.push_back({
                functionLine,
                "Comments",
                "Function has no comments.",
            });
        }
    }
    int childCount = ts_node_child_count(node);
    for (int i = 0; i < childCount; ++i) {
        TSNode child = ts_node_child(node, i);
        visitNode(child, parsedSource, warnings);//check the rest of the file for functions
    }
}

//goes through interior of functions given by visitNode to scan for comments
bool commentChecker::scanForComments(TSNode currentNode){
    if (currentNode.id == nullptr) return false;
                
    std::string_view type(ts_node_type(currentNode));//check current node type
    if (type == "comment") {
        return true; //if comment found, return true
    }
                
    //recursively check children nodes
    int childCount = ts_node_child_count(currentNode);
        for (int i = 0; i < childCount; ++i) {
            if (scanForComments(ts_node_child(currentNode, i))){
                return true;//if a child node has a comment, propagate outwards
            };
        }
    return false;//nothing found
}

//initializes tree sitter, calls visitNode to scan functions and print warnings, and cleans up tree sitter, returning warning counter
std::vector<Warning> commentChecker::analyzeSource(const ParsedSource& parsedSource) {
    std::vector<Warning> warnings;

    // initialize the tree sitter parser
    TSParser *parser = ts_parser_new();
    ts_parser_set_language(parser, tree_sitter_cpp());

    // Parse the raw source string
    const std::string& sourceCode = parsedSource.source;
    TSTree *tree = ts_parser_parse_string(
        parser, 
        nullptr, 
        sourceCode.c_str(), 
        sourceCode.length()
    );

    
    TSNode rootNode = ts_tree_root_node(tree);//extract the root node of the C++ syntax tree

    visitNode(rootNode, parsedSource, warnings);//call visitNode to go through and scan for functions and comments and print warnings

    //clean up tree sitter to avoid memory leaks
    ts_tree_delete(tree);
    ts_parser_delete(parser);

    return warnings;//return the counter
}