#include <iostream>

int normalRepeatBlocks(int a, int b) {
    int result = 0;
    result = a + b;
    result = a - b;
    result = a * b;
    result = a + b;
    result = a - b;
    result = a * b;

    std::string message = " ";

    message += "Hello";
    message += "Hello";
    message += "Hello";
    message += "Hello";
    message += "Hello";
    message += "Hello";
    message += "Hello";

    return result;
}


void shouldBeLoop(){
    int numbers[5] = {}, index = 0;

    numbers[index] += 1;
    index++;

    numbers[index] += 1;
    index++;

    numbers[index] += 1;
    index++;

    numbers[index] += 1;
    index++;

    numbers[index] += 1;
    index++;

    numbers[index] += 1;
    index++;

}

void repeatedButDiffOrder(int a, int b){
    int result = 0;
    result = a + b;
    result = a - b;
    result = a * b;

    result = a * b;
    result = a - b;
    result = a + b;

}

void repeatedButOverlapped(){

    std::string message = " ";

    message += "Hello";
    message += "Hello";
    std::cout << "Hello" << std::endl;
    message += "Hello";
    message += "Hello";

    int foo = 1;
    message += "Hello";
    message += "Hello";
    message += "Hello";


}