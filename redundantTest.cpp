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
int computeStats(int a, int b) {
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