#include <cstdlib>

int basicPassingTest(){
    int* exampleptr = new int;
    delete exampleptr;
    return 1;
}

int basicTest(){
    int* exampleptr = new int;
    return 2;
}

int freePassingTest(){
    int* exampleptr = (int*) std::malloc(5);
    std::free(exampleptr);
    return 1;
}

int multiplePassingTest(){
    int* exampleptr = new int;
    int* exampleptrTwo = new int;
    int* exampleptrThree = new int;
    delete exampleptr;
    delete exampleptrTwo;
    delete exampleptrThree;
    return 2;
}

int multipleTest(){//also tests substring issue
    int* exampleptr = new int;
    int* exampleptrTwo = new int;
    int* exampleptrThree = new int;
    delete exampleptr;
    delete exampleptr;
    delete exampleptr;
    return 2;
}

int conditionalPassingTest(int input){
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

void functionCallPassingTest(){
    int* exampleptr = new int;
    deleterFunction(exampleptr);
    return;
}

void deleterFunction(int* exampleptr){
    delete exampleptr;
    return;
}