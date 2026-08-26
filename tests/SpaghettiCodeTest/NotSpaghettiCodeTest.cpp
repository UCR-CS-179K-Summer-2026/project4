// The class doesn't have 2 long methods that call to each other ot fall to the same field.
// -> Has spaghetti code smell.
#include "NotSpaghettiCodeTest.h"
int NotSpaghettiCodeTest :: func1(){
        int v = this->f1;
        v++;
        v++;
        v++;
        v++;
        v++;
        v++;
        v++;
        v++;
        v++;
        v++;
        v++;
        v++;
        return v;
}
void NotSpaghettiCodeTest :: func2(){
        int v = this->f2;
        v++;
        v++;
        v++;
        v++;
        v++;
        v++;
        v++;
        v++;
        v++;
        v++;
        v++;
        v++;
        v++;
}
void NotSpaghettiCodeTest :: func3(){
        int v = this->f2;
}

void NotSpaghettiCodeTest :: func4(){
        this->func2();
}