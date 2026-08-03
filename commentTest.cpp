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
    std::string testerString = "}}}";
    return num;//previous line has closing brackets so if strings are not stripped the checker will not see this comment
}

int inlineCommentedBrackets(int num){
    //}}}}}
    return num;
}

int blockCommentedBrackets(int num){
    /*}}}there are brackets in this comment*/
    return num;
}