#include "DataClassTest1.h"

// Builds a record total from all values supplied by the data object.
int buildRecordTotal(int firstValue, int secondValue, int thirdValue, int fourthValue, int fifthValue) {
    return firstValue + secondValue + thirdValue + fourthValue + fifthValue;
}

// Checks the record's active state using the simplest boolean form.
bool isRecordActive(bool isActive) {
    return isActive == true;
}
