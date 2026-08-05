#include <iostream>
#include <string>

// Case 1: base case - single unused variable
int calculateCost(int itemPrice, int tax) {
    int processFee = 5;
    int total = itemPrice + tax;
    return total;
}

// Case 2: multiple unused variables in one function
int processOrder(int quantity, int price) {
    int discount = 10;
    int shippingCost = 15;
    int subtotal = quantity * price;
    return subtotal;
}

// Case 3: multi-declarator line with a mix of used and unused
int computeStat(int a, int b) {
    int minVal = a, maxVal = b, average = 0;
    return minVal + maxVal;
}

// Case 4: same variable name reused across different functions
int firstFunction(int input) {
    int counter = 0;
    return input;
}

int secondFunction(int input) {
    int counter = input * 2;
    return counter;
}

// Case 5: unused variable declared inside a nested block (if/for)
int validateInput(int value) {
    if (value > 0) {
        int adjustedValue = value - 1;
        int isValid = 1;
        return isValid;
    }
    return 0;
}

// Case 6: variable declared outside a block, used only inside it
int sumIfPositive(int value) {
    int result = 0;
    if (value > 0) {
        result = value;
    }
    return result;
}

// Case 7: no unused variables at all (confirm no false positives)
int cleanFunction(int a, int b) {
    int sum = a + b;
    int product = a * b;
    return sum + product;
}

// Case 8: commented-out declaration should NOT be flagged
int withCommentedCode(int x) {
    // int oldDebugVar = 99;
    int result = x + 1;
    return result;
}

// Case 9: multiple unused variables spread across several statements
// in the same function, mixed with used ones
int inventoryCheck(int stock, int threshold) {
    int reorderFlag = 0;
    int lastCheckedBy = 7;
    int isLowStock = stock < threshold;
    int notes = 0;
    return isLowStock;
}

// Case 10:
int unusedVariableCases() {
    int a;              // UNUSED: never referenced -> should flag
    int b = 5;           // UNUSED: initialized but never read -> should flag
    int c;
    c = 10;
    std::cout << c;      // USED: should NOT flag

    int x, y = 2;         // UNUSED: x -> should flag; y is used below
    std::cout << y;

    int* ptr = nullptr;   // UNUSED: pointer -> should flag (tests pointer_declarator)
    int& ref = c;         // ref IS used below -> should NOT flag
    std::cout << ref;

    for (int i = 0; i < 10; i++) {  // USED: i is used in condition/increment/body -> should NOT flag
        std::cout << i;
    }

    {
        int nested = 42;  // UNUSED, but inside a nested block -> should still flag (tests recursive scan)
    }

    return 0;
}

// Case 11: boolean comparison cases
void booleanComparisonCases(bool isValid, bool isReady, int count) {
    if (isValid == true) {}     // should flag -> simplifies to "isValid"
    if (isValid == false) {}    // should flag -> simplifies to "!isValid"
    if (isValid != true) {}     // should flag -> simplifies to "!isValid"
    if (isValid != false) {}    // should flag -> simplifies to "isValid"

    if (true == isReady) {}     // should flag (literal on LEFT side -> tests that branch)

    if (isValid == isReady) {}  // should NOT flag: neither side is a bool literal
    if (count == 5) {}          // should NOT flag: int comparison, no bool literal involved
    if (isValid) {}             // should NOT flag: no comparison at all

    bool flag = (isValid == true);  // should flag even inside a nested assignment expression
}

// Case 12: Combined cases
int main1() {
    int price = 5;
    int quantity = 2;
    bool isValid;

    std::string orderStatus;
    bool isReady;


    if(price > 0) {
        isValid = true;
        char c;
    } else {
        isValid = false;
    }

    if(isValid == true) {
        int totalCost = processOrder(quantity, price);
        return totalCost;
    } 
    else if(isValid == false) {
        return price;
    }

    bool isAvailable = true;

    if(quantity > 0 && isAvailable == true) {
        return quantity;
    }else if(quantity <= 0 && isAvailable == false) {
        return 0;
    }
    return 0;
}