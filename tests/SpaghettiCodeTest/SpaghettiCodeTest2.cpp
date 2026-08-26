#include "SpaghettiCodeTest2.h"

// Repeats the same preparation sequence inline.
int SpaghettiCodeTest2::func1() {
    int counter = this->f1;
    counter += 1;
    counter += 2;
    counter += 3;
    counter += 1;
    counter += 2;
    counter += 3;
    return counter;
}

// Repeats the same follow-up sequence inline.
void SpaghettiCodeTest2::func2() {
    int counter = this->func1();
    counter += 1;
    counter += 2;
    counter += 3;
    counter += 1;
    counter += 2;
    counter += 3;
}
