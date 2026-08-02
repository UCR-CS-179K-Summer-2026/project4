#include "commentChecker.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <regex>
#include <string>
#include <vector>


int commentChecker::analyzeSource(const ParsedSource& parsedSource){
    
    int warningCounter = 0;
    std::vector<std::string> lines;//will contain input file as a vector of string lines
    std::string line;//will hold each line as it gets added to fileContent


    //go throught parsedSource line by line
    std::istringstream stream(parsedSource.source);
    while (std::getline(stream, line)) {
        lines.push_back(line);
    }


    std::regex functionRegex(R"(^[a-zA-Z_][a-zA-Z0-9_<>\s\*\&]*\s+[a-zA-Z_][a-zA-Z0-9_]*\s*\([^\)]*\))");//regex to find functions
    std::regex commentRegex(R"((\/\/|\/\*|\*))");//regex to find comments

    
    //iterate through lines vector, searches for function within each line
    for (int i = 0; i < lines.size(); ++i) {

        
        if (std::regex_search(lines[i], functionRegex)) {//detects function, assumes no comment until proven otherwise
            
            int functionLine = i + 1;//keeps track of function line for printing warning, +1 because i starts at 0
            bool hasComment = false;//flag for if there is a comment


            //if i is not the first line of the file, checks if there is a preceding comment
            if (i > 0) {
                for (int j = i - 1; j >= 0; --j) {
                    if (lines[j].find_first_not_of(" \t\r\n") == std::string::npos) {//skip empty lines/lines with only whitespace
                        continue; 
                    }
                    if (std::regex_search(lines[j], commentRegex)) {//if the first non-empty line is a comment, set flag is true
                        hasComment = true;
                    }
                    break; //break to stop from going back past the first non empty line
                }
            }
            

            if (std::regex_search(lines[i], commentRegex)) {//checks for inline comment
                
                hasComment = true;
            }

            
            //search function body line by line for comments, using brace height to detect start and end
            int insideFunction = false;//flag for use when scanning function interior
            int braceDepth = 0;//for counting unclosed braces, used to find when the function ends
            int k = i;//iterator for inside function
            while(k < lines.size()){

                //for loop goes through current line to find braces
                for(char ch : lines[k]){
                    
                    if (ch == '{'){
                        
                        braceDepth++;
                        insideFunction = true;//after first brace means inside function body
                    }
                    else if(ch == '}'){//closing one of the open braces means one less brace depth
                        
                        braceDepth--;
                    }
                }

                if(std::regex_search(lines[k], commentRegex)) {//check if current line has a comment
                    
                    hasComment = true;
                }

                if(insideFunction && braceDepth <= 0){//if braceDepth reaches 0 after having entered the function, then the function has ended
                    
                    break;
                }

                k++;//increment k to the next line
            }
            if (insideFunction && k < lines.size()) {//if we entered a function, and didn't reach the end of the file yet, set i = k so we don't rescan the lines between
                
                    i = k; 
            }
            if(!hasComment){//if no comments detected, add counter and print warning

                std::cout << "Warning: function in line " << functionLine << " has no comments" << std::endl;
                warningCounter++;
            }
        }

    }

    return warningCounter;
}