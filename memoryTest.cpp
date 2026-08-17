int passingTest(){
    int* exampleptr = new int;
    delete exampleptr;
    return 1;
}

int basicTest(){
    int* exampleptr = new int;
    return 2;
}

int conditionalTest(int input){
    int* exampleptr = new int;
    if(input = 1){
        return;
    }
    delete exampleptr;
    return;
}

int conditionalTestTwo(int input){
    int* exampleptr = new int;
    if(input = 1){
        delete exampleptr;
        return;
    }
    return;
}