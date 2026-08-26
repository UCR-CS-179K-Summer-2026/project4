#include "DataClassTest2.h"

// Processes nested record validation rules.
int processRecord(int recordValue) {
    int result = recordValue;
    if (recordValue > 0) {
        if (recordValue > 1) {
            if (recordValue > 2) {
                if (recordValue > 3) {
                    result += 1;
                }
            }
        }
    }
    result += 1;
    result += 2;
    result += 3;
    result += 1;
    result += 2;
    result += 3;
    return result;
}
