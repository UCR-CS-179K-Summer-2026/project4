#include <cstdlib>

int basicPassingTest(){//basic test, should pass
    int* exampleptr = new int;
    delete exampleptr;
    return 1;
}

int basicTest(){//basic test, should fail (line 10)
    int* exampleptr = new int;
    return 2;
}

int freePassingTest(){//test with free(), should pass
    int* exampleptr = (int*) std::malloc(5);
    std::free(exampleptr);
    return 1;
}

int multiplePassingTest(){//test with multiple, should pass
    int* exampleptr = new int;
    int* exampleptrTwo = new int;
    int* exampleptrThree = new int;
    delete exampleptr;
    delete exampleptrTwo;
    delete exampleptrThree;
    return 2;
}

int conditionalPassingTest(int input){//test with conditional, should pass
    int* exampleptr = new int;
    if(input == 1){
        delete exampleptr;
        return 1;
    }
    delete exampleptr;
    return 1;
}

int conditionalTest(int input){//conditional test, inside if does not deallocate, fail on line 41
    int* exampleptr = new int;
    if(input == 1){
        return 1;
    }
    delete exampleptr;
    return 1;
}

int conditionalTestTwo(int input){//conditional test, outside if does not deallocate, fail on line 50
    int* exampleptr = new int;
    if(input == 1){
        delete exampleptr;
        return 1;
    }
    return 1;
}

void functionCallPassingTest(){//test with function deallocation, should pass
    int* exampleptr = new int;
    deleterFunction(exampleptr);
    return;
}

void deleterFunction(int* exampleptr){//deleter function for previous test
    delete exampleptr;
    return;
}

void functionCallFailingTest(){//test with function call that does not deallocate, fail on line 70
    int* exampleptr = new int;
    nonDeleterFunction(exampleptr);
    return;
}

void nonDeleterFunction(int* exampleptr){//"deleter" function with nothing in it for previous test
    return;
}