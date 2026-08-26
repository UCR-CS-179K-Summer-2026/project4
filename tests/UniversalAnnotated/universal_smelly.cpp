#include <iostream>
#include <string>

// Repeated parameter groups exercise the data-clump detector.
void clumpOne(std::string firstName, std::string lastName, std::string city) {
    std::cout << firstName << lastName << city << std::endl;
}

void clumpTwo(std::string firstName, std::string lastName, std::string city) {
    std::cout << firstName << lastName << city << std::endl;
}

void clumpThree(std::string firstName, std::string lastName, std::string city) {
    std::cout << firstName << lastName << city << std::endl;
}

// Intentionally has no descriptive comment immediately before the function.
int universalSmellyMain() {
    int x = 0;
    bool ready = true;
    if (ready == true) {
        x = x + 1;
    }

    x = x + 1;
    x = x + 2;
    x = x + 3;
    x = x + 1;
    x = x + 2;
    x = x + 3;

    if (x > 0) {
        if (x > 1) {
            if (x > 2) {
                if (x > 3) {
                    x++;
                }
            }
        }
    }

    clumpOne("A", "B", "C");
    clumpTwo("A", "B", "C");
    clumpThree("A", "B", "C");
    return x;
}

int tooManyParameters(int first, int second, int third, int fourth, int fifth) {
    return first + second + third + fourth + fifth;
}

void leakedAllocation() {
    int* leaked = new int;
    std::cout << *leaked << std::endl;
}

class BaseUniversal {
public:
    virtual void keep() {}
};

class UnusedBaseUniversal : public BaseUniversal {
public:
    void ownWork() {}
};

void unusedFunction() {}

int main() {
    std::cout << universalSmellyMain() << std::endl;
    std::cout << tooManyParameters(1, 2, 3, 4, 5) << std::endl;
    leakedAllocation();
    UnusedBaseUniversal value;
    value.ownWork();
    return 0;
}
