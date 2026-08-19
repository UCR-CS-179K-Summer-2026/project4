#include <iostream>
#include <string>

// ==================================================================
// Check 1: Unused / Dead Variables
// ==================================================================

// Base case: one unused var next to one used var
int basicUnusedVariable(int itemPrice, int tax) {
              // UNUSED -> should flag
    int total = itemPrice + tax; // USED
    return total;
}

// Multiple unused vars + a multi-declarator line with mixed used/unused
int multipleUnusedAndMultiDeclarator(int quantity, int price) {
          // UNUSED -> should flag
      // UNUSED -> should flag
    int minVal = quantity, maxVal = price, average = 0; // average UNUSED -> should flag
    int subtotal = quantity * price;
    return subtotal + minVal + maxVal;
}

// Same variable name reused across separate functions: each function's scope is independent
int firstFunction(int input) {
             // UNUSED -> should flag (never read in this function)
    return input;
}
int secondFunction(int input) {
    int counter = input * 2; // USED -> should NOT flag
    return counter;
}

// Unused var nested inside a block, vs. an outer var only ever used inside a block
int nestedBlockCases(int value) {
    int result = 0;
    if (value > 0) {
         // UNUSED, nested in if-branch -> should flag
        result = value;                // outer var used inside block -> should NOT flag
    }
    return result;
}

// Negative control: everything declared is read somewhere -> no false positives
int cleanNoFalsePositives(int a, int b) {
    int sum = a + b;
    int product = a * b;
    return sum + product;
}

// Commented-out declarations must never be flagged (parser shouldn't see them)
int commentedCodeIgnored(int x) {
    // int oldDebugVar = 99;
    /* int oldDebugVar2 = 100; */
    int result = x + 1;
    return result;
}

// Declarator-type coverage in one place: plain, pointer, reference, loop var, nested-scope scan
int declaratorTypeCoverage() {
                 // UNUSED -> should flag
              // UNUSED -> should flag
    int c;
    c = 10;
    std::cout << c;     // USED -> should NOT flag

    int x, y = 2;         // x UNUSED -> should flag; y used below -> should NOT flag
    std::cout << y;

       // UNUSED pointer -> should flag
    int& ref = c;         // USED below -> should NOT flag
    std::cout << ref;

    for (int i = 0; i < 10; i++) {  // loop var used in condition/body -> should NOT flag
        std::cout << i;
    }

    {
          // UNUSED inside a nested {} block -> should still flag
    }

    return 0;
}

// Shadowing: same name re-declared in nested scopes must be tracked independently
void shadowingAcrossScopes() {
    int x = 5;       // UNUSED in this (outer) scope -> should flag
    while (2 - 2) {
        int x = 5;    // inner shadow, USED below -> should NOT flag
        std::cout << x;
    }
            // UNUSED -> should flag
    if (x > 2) {
              // UNUSED shadow -> should flag
        if (x > 1) {
            int y = 4;   // USED below -> should NOT flag
            std::cout << x;
            std::cout << y;
        }
    }
    // Negative control: no unused vars in this loop
    int val = 10;
    while (val > 1) {
        int z = 2;
        val--;
        if (z > 1) {
            z--;
            val--;
        }
    }
}

// ==================================================================
// Check 2: Redundant Boolean Comparisons
// ==================================================================
void booleanComparisonCases(bool isValid, bool isReady, int count) {
    if (isValid) {}     // should flag -> simplifies to "isValid"
    if (!isValid) {}    // should flag -> simplifies to "!isValid"
    if (!isValid) {}     // should flag -> simplifies to "!isValid"
    if (isValid) {}    // should flag -> simplifies to "isValid"

    if (isReady) {}     // should flag (literal on LEFT side -> tests that branch)

    if (isValid == isReady) {}  // should NOT flag: neither side is a bool literal
    if (count == 5) {}          // should NOT flag: int comparison, no bool literal involved
    if (isValid) {}             // should NOT flag: no comparison at all

    bool flag = (isValid);  // should flag even inside a nested assignment expression
}

void booleanComparisons_nestedAndNegative(bool isReady, bool isAvailable, int quantity) {
    if (quantity > 0 && isAvailable) {}   // flag: "isAvailable == true" -> "isAvailable"

    if (isReady == isAvailable) {}   // should NOT flag: neither side is a bool literal
    if (quantity == 5) {}            // should NOT flag: int comparison
    if (isReady) {}                  // should NOT flag: no comparison at all

    bool flag = (isReady);   // flag even inside an assignment expression
}

// ==================================================================
// Check 3: Chained If-Statements That Should Be If/Else If/Else
// ==================================================================
std::string numType(int number) {
    if (number > 0) { return "positive"; }   // should flag: 2+ consecutive ifs, each unconditional return
    if (number == 0) { return "zero"; }
    if (number < 0) { return "negative"; }
    return "";
}

std::string roleAssign(int number, int time) {
    if (number < 10) { return "volunteer"; }              // should flag
    if (number >= 10 && number < 20) { return "committee"; }
    if (number >= 20) { return "leader"; }

    if (time < 5) { return "junior"; }
    else if (time >= 5 && time < 10) { return "senior"; }
    else if (time >= 10) { return "expert"; }
    else { return "unknown"; }
}

int chainedReturns_singleIf(int x) {   // should NOT flag: only 1 if, no chain
    if (x > 0) { return 1; }
    return 0;
}

int chainedReturns_brokenByOtherStatement(int x) {   // should NOT flag: adjacency broken
    if (x > 0) { return 1; }
    std::cout << "checking x\n";   // breaks the chain
    if (x < 0) { return -1; }
    return 0;
}

int chainedReturns_hasElseIf(int x) {   // should NOT flag: already if/else if/else
    if (x > 0) { return 1; }
    else if (x < 0) { return -1; }
    else { return 0; }
}

// ==================================================================
// Check 4: Redundant If/Else Returning Boolean Literals
// ==================================================================
bool caseA(bool cond) {
    return cond;          // should flag -> simplifies to "return cond;"
}

bool caseB(bool cond) {
    return !cond;           // should flag -> simplifies to "return !cond;"
}

bool caseC(bool cond) {         // braced form -> tests compound_statement unwrapping
    return cond;
}

bool caseD(bool cond) {         // should NOT flag: both branches return the SAME literal
    if (cond) return true;
    else return true;
}

bool caseE(bool cond, int x) {  // should NOT flag: else branch isn't a bool literal
    if (cond) return true;
    else return x > 0;
}

int caseF(bool cond) {          // should NOT flag: no else at all
    if (cond) return 1;
    return 0;
}
// ==================================================================
// Check 6: Combined Cases
// ==================================================================
int combined_orderStatusExample(int price, int quantity) {
    bool isValid;
    bool isAvailable = true;      // used below -> should NOT flag as unused

    if (price > 0) {
        isValid = true;
              // UNUSED, nested inside an if-branch -> flag
    } else {
        isValid = false;
    }
    // isValid assigned in both branches, then read below -> should NOT flag as unused

    if (isValid) {                              // flag: boolean comparison
        int totalCost = quantity * price;                  // used below -> should NOT flag
        return totalCost;
    } else if (!isValid) {                       // flag: boolean comparison
        return price;
    }

    if (quantity > 0 && isAvailable) {            // flag: boolean comparison, nested in &&
        return quantity;
    } else if (quantity <= 0 && !isAvailable) {   // flag: boolean comparison
        return 0;
    }

    // Deliberately hits check 2 AND check 3 on adjacent lines
    bool cond = true;                    // cond used below -> should NOT flag as unused
    if (cond) return true;       // flag (check 2): "cond == true" -> "cond"
                                          // flag (check 3, whole if/else): -> "return cond == true;"
    else return false;

    return 0;
}