#include <string>
#include <vector>

/*18 poor variable names in this file */

int calc(int num1, int num2, int arr[]) { // Example with poor function name (poor array name and poor function name)
    int x = 5;  // Example variable with a poor name in function scope
    return num1 * num2 * x;
}

int calculateSum() {
    int sum = 0; 
    for (int i = 0; i < 10; ++i) { // "i" will not be flagged
        int a = i; // Example varaible with a poor name in loop scope (less than 3 letters)
        sum += a;
    }
    
    return sum;
}

bool isAdmin(const std::string& username, const std::string& password) {
    bool flag = false; // Example variable with a poor name in function scope (does not have boolean prefix)
    if (username == "admin" && password == "password") {
        flag = true;
    }

    return flag;
}

void swapData() {
    int xy, data = 10; // Example variables with poor names in function scope (less than 3 leters and too generic name. Should flag both on the same line)
    xy = 20;
    std::vector<int> vec; // Example vector with a poor name in function scope (poor vector name)
    
    int temp = data; // Example variable with a poor name in function scope (generic temporary variable name)
    data = xy;
    xy = temp;
}

void nestedLoop() {
    int m = 0; // Example variable with a poor name in function scope (less than 3 letters)
    int n = 0; // Example variable with a poor name in function scope (less than 3 letters)

    for (int i = 0; i < 10; ++i) { // "i" will not be flagged
        m++;
        for (int j = 0; j < 5; ++j) {
            n++;
        }
    }
}

int addThreeNumbers() {
    int arr[10]; // Example array with a poor name in function scope (poor array name)
    int x = 1; // Example variable with a poor name in function scope (less than 3 letters)
    int y = 2; // Example variable with a poor name in function scope (less than 3 letters)
    int z = 3; // Example variable with a poor name in function scope (less than 3 letters)

    int sum = x + y + z;
    return sum;
}

// Example function with poor variable names in parameters (all less than 3 letters)
int addThreeNumbers(const int& a, const int& b, const int& c) {
    return a + b + c;
}
