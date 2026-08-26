// The class has at least 2 long methods that call to same field. (long method= longer than the average in the class)
// -> Has spaghetti code smell.
#include "SpaghettiCodeTest1.h"
void SpaghettiCodeTest1 :: func1(){
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
        v++;
}
void SpaghettiCodeTest1 :: func2(){
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
        v++;
}