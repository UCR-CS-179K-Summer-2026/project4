int basicPassingTest(){
    int* exampleptr = new int;
    delete exampleptr;
    return 1;
}

int basicTest(){
    int* exampleptr = new int;
    return 2;
}

conditionalPassingTest(int input){
    int* exampleptr = new int;
    if(input == 1){
        delete exampleptr;
        return 1;
    }
    delete exampleptr;
    return 1;
}

int conditionalTest(int input){
    int* exampleptr = new int;
    if(input == 1){
        return 1;
    }
    delete exampleptr;
    return 1;
}

int conditionalTestTwo(int input){
    int* exampleptr = new int;
    if(input == 1){
        delete exampleptr;
        return 1;
    }
    return 1;
}

void functionCallTest(){
    int* exampleptr = new int;
    deleterFunction(exampleptr);
    return;
}

void deleterFunction(int* exampleptr){
    delete exampleptr;
    return;
}