#include <iostream>
#include <fstream>
#include <regex>
#include <string>
#include <vector>


int commentChecker(std::ifstream& inputFile){
    
    int warningCounter = 0;

    if (!inputFile.is_open()) {//check if inputFile is reading
        
        std::cout << "Error: commentChecker could not open file " << std::endl;
        return 0;
    }

    std::vector<std::string> lines;//will contain input file as a vector of string lines
    std::string line;//will hold each line as it gets added to fileContent

    while (std::getline(inputFile, line)){//copy inputFile's contents into lines vector one line at a time
        
        lines.push_back(line);
    }

    std::regex functionRegex(R"(^[a-zA-Z_][a-zA-Z0-9_<>\s\*\&]*\s+[a-zA-Z_][a-zA-Z0-9_]*\s*\([^\)]*\))");//regex to find functions
    std::regex commentRegex(R"((\/\/|\/\*|\*))");//regex to find comments


    for (int i = 0; i < lines.size(); ++i) {//iterate through lines vector

        if (std::regex_search(lines[i], functionRegex)) {//searches for function within the line, assumes no comment until proven otherwise
            
            int functionLine = i + 1;//keeps track of function line for printing warning, +1 because i starts at 0
            bool hasComment = false;//flag for if there is a comment
            
            if (std::regex_search(lines[i], commentRegex)) {//checks for inline comment
                
                hasComment = true;
            }

            //reset these two
            int insideFunction = false;//flag for use when scanning function interior
            int braceDepth = 0;//for counting unclosed braces, for finding when the function ends
            int k = i;//iterator for inside function
            while(k < lines.size()){//search function body for comments, using brace height to detect start and end
                
                for(char ch : lines[k]){//for loop going through current line to find braces
                    
                    if (ch == '{'){
                        
                        braceDepth++;
                        insideFunction = true;//after first brace means inside function body
                    }
                    else if(ch == '}'){//closing brace means one less brace depth
                        
                        braceDepth--;
                    }
                }

                if(std::regex_search(lines[k], commentRegex)) {//check if this line has a comment
                    
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