#include <string>
#include <vector>

int multiply(int num1, int num2, int arr[]) {
    int x = 5;  // Example variable with a poor name in function scope
    return num1 * num2 * x;
}

int calculateSum() {
    int sum = 0; 
    for (int i = 0; i < 10; ++i) {
        int a = i; // Example varaible with a poor name in loop scope
        sum += a;
    }
    
    return sum;
}

bool isAdmin(const std::string& username, const std::string& password) {
    bool flag = false; // Example variable with a poor name in function scope
    if (username == "admin" && password == "password") {
        flag = true;
    }

    return flag;
}

void swapData() {
    int data = 10; // Example variable with a poor name in function scope
    int newNumber = 20;
    std::vector<int> vec;
    
    int temp = data; // Example variable with a poor name in function scope
    data = newNumber;
    newNumber = temp;
}

void nestedLoop() {
    int m = 0; // Example variable with a poor name in function scope
    int n = 0; // Example variable with a poor name in function scope

    for (int i = 0; i < 10; ++i) {
        m++;
        for (int j = 0; j < 5; ++j) {
            n++;
        }
    }
}

int addThreeNumbers() {
    int arr[10];
    int x = 1; // Example variable with a poor name in function scope
    int y = 2; // Example variable with a poor name in function scope
    int z = 3; // Example variable with a poor name in function scope

    int sum = x + y + z;
    return sum;
}

// Example function with poor variable names in parameters
int addThreeNumbers(const int& a, const int& b, const int& c) {
    return a + b + c;
}

/* EXPECTED CODE SMELLS: 13 */
