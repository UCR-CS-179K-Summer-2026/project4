#include <string>


int uncommentedFunction(int num) {
    return num;
}

//comment before function with whitespace


int commentedBeforeFunction(int num){
    return num;
}

int commentedInlineFunction(int num){//comment inline
    return num;    
}

int commentedInFunction(int num){
    return num;//comment in function body
}

int commentEndOfFunction(int num){
    return num;
}//comment on end bracket line


int commentedAfterBracketString(int num){
    std::string testerString = "{{{";//line has open brackets so if strings are not stripped the checker will treat the rest of the file as one function and not see the commentless function at the end of this test file
    return num;
}

int commentedAfterBracketChar(int num){
    char testerChar = '{';//line has open brackets so if chars are not stripped the checker will treat the rest of the file as one function and not detect the commentless function at the end of this test file
    return num;
}

int inlineCommentedBrackets(int num){
    if(true){//{{tests for same logic as above cases except for comments; includes an uncommented bracket on the same line to see if that breaks things
        return num;
    }
    return num;
}

int blockCommentedBrackets(int num){
    if(true){/*{{same as above for block comments*/
        return num;
    }
    return num;
}

int literalChecker(int num){
    return num;
}