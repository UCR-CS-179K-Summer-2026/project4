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
    std::string testerString = "//";//line has comment in string
    return num;
}

int inlineCommentedBrackets(int num){
    if(true){
        if(true){
            if(true){
                if(true){
                    return num;//comment in nested ifs
                }
            }
        }
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