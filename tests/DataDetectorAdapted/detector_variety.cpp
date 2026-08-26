#include <iostream>
#include <string>

// Collects the same record data group used by the other data functions.
void prepareRecord(std::string firstName, std::string lastName, std::string city) {
    std::cout << firstName << lastName << city << std::endl;
}

// Collects the same record data group for a report.
void printRecord(std::string firstName, std::string lastName, std::string city) {
    std::cout << firstName << lastName << city << std::endl;
}

// Collects the same record data group for storage.
void storeRecord(std::string firstName, std::string lastName, std::string city) {
    std::cout << firstName << lastName << city << std::endl;
}

// Exercises the long-parameter-list detector.
int calculateRecord(int itemCount, int unitPrice, int discount, int tax, int shipping) {
    return itemCount * unitPrice - discount + tax + shipping;
}

// Exercises redundant boolean comparison.
bool acceptsRecord(bool isAvailable) {
    return isAvailable == true;
}

// Exercises repeated statements inside one function.
int repeatRecordSteps(int recordCount) {
    recordCount += 1;
    recordCount += 2;
    recordCount += 3;
    recordCount += 1;
    recordCount += 2;
    recordCount += 3;
    return recordCount;
}

// Exercises deeply nested conditional handling.
int inspectRecord(int recordCount) {
    int result = recordCount;
    if (recordCount > 0) {
        if (recordCount > 1) {
            if (recordCount > 2) {
                if (recordCount > 3) {
                    result += 1;
                }
            }
        }
    }
    return result;
}

// Exercises the allocation checker.
int* allocateRecordCount() {
    int* recordCount = new int(1);
    return recordCount;
}

// This function is intentionally unreachable from main.
void unusedRecordHelper() {}

class RecordBase {
public:
    // Dispatches a record for the base interface.
    virtual void dispatch() {}
};

class UnusedRecordInheritance : public RecordBase {
public:
    // Prepares the record without using the base interface.
    void prepare() {}
};

// Runs the supported detector variety case.
int main() {
    prepareRecord("A", "B", "C");
    printRecord("A", "B", "C");
    storeRecord("A", "B", "C");
    std::cout << calculateRecord(2, 10, 1, 2, 3) << std::endl;
    std::cout << acceptsRecord(true) << std::endl;
    std::cout << repeatRecordSteps(0) << std::endl;
    std::cout << inspectRecord(4) << std::endl;
    UnusedRecordInheritance record;
    record.prepare();
    return *allocateRecordCount();
}
