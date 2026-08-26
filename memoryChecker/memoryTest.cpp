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

int multipleTest(){//test with multiple, should fail lines 32 and 33
    int* exampleptr = new int;
    int* exampleptrTwo = new int;
    int* exampleptrThree = new int;
    delete exampleptr;
    delete exampleptr;
    delete exampleptr;
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

int conditionalTest(int input){//conditional test, inside if does not deallocate, fail on line 51
    int* exampleptr = new int;
    if(input == 1){
        return 1;
    }
    delete exampleptr;
    return 1;
}

int conditionalTestTwo(int input){//conditional test, outside if does not deallocate, fail on line 60
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

void functionCallFailingTest(){//test with function call that does not deallocate, fail on line 80
    int* exampleptr = new int;
    nonDeleterFunction(exampleptr);
    return;
}

void nonDeleterFunction(int* exampleptr){//"deleter" function with nothing in it for previous test
    return;
}

void multipleFunctionCallPassingTest(){//test with multiple function deallocation including name changes, should pass
    int* exampleptr = new int;
    int* exampleptrTwo = new int;
    multipleDeleterFunction(exampleptr, exampleptrTwo);
    return;
}

void multipleDeleterFunction(int* one, int* two){//function that deletes multiple params, reference by different names from the original
    delete one;
    delete two;
}

void multipleFunctionCallPartialTest(){//test with multiple function call that only partially deletes params, should fail on line 103
    int* exampleptr = new int;
    int* exampleptrTwo = new int;
    partialDeleterFunction(exampleptr, exampleptrTwo);
    return;
}

void partialDeleterFunction(int* one, int* two){//function that takes multiple params and only deletes some, reference by different names from the original
    delete one;
}

void foo(int* a);
void bar(int* b) {
    foo(b);
}
void foo(int* a) {
    delete a;
}

void testBar(int* c) {
    bar(c);
}