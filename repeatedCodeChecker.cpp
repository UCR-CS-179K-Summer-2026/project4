#include "repeatedCodeChecker.h"

#include <iostream>
#include <sstream>
#include <regex>
#include <algorithm>
#include <unordered_map>

std::string RepeatedCodeChecker::stripBlockComments(const std::string& content) const {

}

std::string RepeatedCodeChecker::stripLineComment(const std::string& line) const{

}

std::string RepeatedCodeChecker::normalizeWhitespace(const std::string& line) const{

}

bool RepeatedCodeChecker::isStructuralOnly(const std::string& line) const{

}
 
std::vector<RepeatedCodeChecker::codeLine> RepeatedCodeChecker::extractCodeLines(const std::string& content) const{

}
 
void RepeatedCodeChecker::reportRepeatedBlock(const std::vector<codeLine>& lines,int windowSize,const std::vector<int>& startIndices) const{
    
}
