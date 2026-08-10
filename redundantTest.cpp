#include <iostream>
#include <string>

// --------- Check 1: Unused/Dead Variables --------- 

// single unused variable 
int calculateCost(int itemPrice, int tax) {
    int processFee = 5;         // UNUSED: -> should flag
    int total = itemPrice + tax;
    return total;
}

// multiple unused variables in one function
int processOrder(int quantity, int price) {
    int discount = 10;          // UNUSED: -> should flag
    int shippingCost = 15;      // UNUSED: -> should flag
    int subtotal = quantity * price;
    return subtotal;
}

// multi-declarator line with a mix of used and unused
int computeStat(int a, int b) {
    int minVal = a, maxVal = b, average = 0;
    return minVal + maxVal;
}

// same variable name reused across different functions
int firstFunction(int input) {
    int counter = 0;        // UNUSED: -> should flag
    return input;
}

int secondFunction(int input) {
    int counter = input * 2;    // USED: should NOT flag
    return counter;
}

// unused variable declared inside a nested block (if/for)
int validateInput(int value) {
    if (value > 0) {
        int adjustedValue = value - 1;  // UNUSED: -> should flag
        int isValid = 1;
        return isValid;
    }
    return 0;
}

// variable declared outside a block, used only inside it
int sumIfPositive(int value) {
    int result = 0;
    if (value > 0) {
        result = value;
    }
    return result;
}

// no unused variables at all (confirm no false positives)
int cleanFunction(int a, int b) {
    int sum = a + b;
    int product = a * b;
    return sum + product;
}

// commented-out declaration should NOT be flagged
int withCommentedCode(int x) {
    // int oldDebugVar = 99;
    /* int oldDebug Var = 100 */
    int result = x + 1;
    return result;
}

// multiple unused variables spread across several statements
// in the same function, mixed with used ones
int inventoryCheck(int stock, int threshold) {
    int reorderFlag = 0;    // UNUSED: -> should flag
    int lastCheckedBy = 7;  // UNUSED: -> should flag
    int isLowStock = stock < threshold;
    int notes = 0;          // UNUSED: -> should flag
    return isLowStock;
}

// Unused Variables in Different Contexts
int unusedVariableCases() {
    int a;               // UNUSED: never referenced -> should flag
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

// --------- Check 2: Redundant Boolean Comparisons ---------
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

void booleanComparisons_nestedAndNegative(bool isReady, bool isAvailable, int quantity) {
    // nested inside && -> should still be found by traversal
    if (quantity > 0 && isAvailable == true) {}   // flag: "isAvailable == true" -> "isAvailable"

    if (isReady == isAvailable) {}   // should NOT flag: neither side is a bool literal
    if (quantity == 5) {}            // should NOT flag: int comparison
    if (isReady) {}                  // should NOT flag: no comparison at all

    bool flag = (isReady == true);   // flag even inside an assignment expression, not just if-conditions
}

// --------- Check 3: Redundant if/else if/ else conditional statements ---------
std:: string numType(int number) {
    if (number > 0) { return "positive"; }      // should flag, recognizes 2+ consecutive ifs with unconditional returns
    if (number == 0) { return "zero"; }
    if (number < 0) { return "negative"; }
}

std:: string roleAssign(int number, int time) {
    if (number < 10) { return "volunteer"; }    // should flag, recognizes 2+ consecutive ifs with unconditional returns
    if (number >= 10 && number < 20) { return "committee"; }
    if (number >= 20) { return "leader"; }

    if (time < 5) { return "junior"; }
    else if (time >= 5 && time < 10) { return "senior";} 
    else if (time >= 10) { return "expert"; }
    else { return "unknown"; }
}

int chainedReturns_singleIf(int x) {   // should NOT flag: only 1 if, no chain
    if (x > 0) { return 1; }
    return 0;
}

int chainedReturns_brokenByOtherStatement(int x) {   // should NOT flag as one chain: statement breaks adjacency
    if (x > 0) { return 1; }
    std::cout << "checking x\n";   // breaks the chain
    if (x < 0) { return -1; }
    return 0;
}

int chainedReturns_hasElseIf(int x) {   // should NOT flag: already written as if/else if/else, not separate ifs
    if (x > 0) { return 1; }
    else if (x < 0) { return -1; }
    else { return 0; }
}

// --------- Check 4: Redundant If/Else Returning Boolean Literals --------- 
bool caseA(bool cond) {
    if (cond) return true;
    else return false;          // should flag -> simplifies to "return cond;"
}

bool caseB(bool cond) {
    if (cond) return false;
    else return true;           // should flag -> simplifies to "return !cond;"
}

bool caseC(bool cond) {         // braced form -> tests compound_statement unwrapping
    if (cond) {
        return true;
    } else {
        return false;
    }
}

bool caseD(bool cond) {         // should NOT flag: both branches return the SAME literal
    if (cond) return true;
    else return true;
}

bool caseE(bool cond, int x) {  // should NOT flag: else branch doesn't return a bool literal
    if (cond) return true;
    else return x > 0;
}

int caseF(bool cond) {          // should NOT flag: no else at all
    if (cond) return 1;
    return 0;
}

// --------- Check 5: Combined Cases --------- 
int combined_orderStatusExample(int price, int quantity) {
    bool isValid;
    bool isAvailable = true;      // used below -> should NOT flag as unused

    if (price > 0) {
        isValid = true;
        char unusedInBranch;      // UNUSED, nested inside an if-branch -> flag
    } else {
        isValid = false;
    }
    // isValid assigned in both branches, then read below -> should NOT flag as unused

    if (isValid == true) {                          // flag: boolean comparison
        int totalCost = quantity * price;             // used below -> should NOT flag
        return totalCost;
    } else if (isValid == false) {                   // flag: boolean comparison
        return price;
    }

    if (quantity > 0 && isAvailable == true) {        // flag: boolean comparison, nested in &&
        return quantity;
    } else if (quantity <= 0 && isAvailable == false) { // flag: boolean comparison
        return 0;
    }

    // Deliberately hits check 2 AND check 3 on adjacent lines
    bool cond = true;                    // cond used below -> should NOT flag as unused
    if (cond == true) return true;       // flag (check 2): "cond == true" -> "cond"
                                          // flag (check 3, whole if/else): -> "return cond == true;"
    else return false;

    return 0;
}

void foo() {
    int x = 5;
    while (2-2) {
        int x = 5;
        std::cout << x;
    }
}