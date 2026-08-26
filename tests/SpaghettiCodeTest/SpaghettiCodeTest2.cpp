// The class has at least 2 long methods that call to each other. (long method= longer than the average in the class)
// -> Has spaghetti code smell.
#include "SpaghettiCodeTest2.h"

int SpaghettiCodeTest2 :: func1(){
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
void SpaghettiCodeTest2 :: func2(){
        int v = this->func1();
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