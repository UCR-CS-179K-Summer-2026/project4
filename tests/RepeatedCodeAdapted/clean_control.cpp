#include "clean_control.h"

// Performs one focused inventory update.
int NotSpaghettiCodeTest::func1() {
    return this->f1 + 1;
}

// Performs one focused reservation update.
void NotSpaghettiCodeTest::func2() {
    this->f2 += 1;
}

// Reads the current reservation count.
void NotSpaghettiCodeTest::func3() {
    int reservationCount = this->f2;
    (void) reservationCount;
}

// Delegates one operation through a named method.
void NotSpaghettiCodeTest::func4() {
    this->func2();
}
