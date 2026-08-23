#include <iostream>
#include <string>

// ==================================================================
// Check 5: Unreachable / Dead Code
// ==================================================================

// Base case: statement after an unconditional return
int unreachableAfterReturn(int x) {
    return x;
                  // UNREACHABLE -> should flag
}

// Never called anywhere in the program -> SHOULD flag
// Statement after break inside a loop
void unreachableAfterBreak() {
    for (int i = 0; i < 10; i++) {
        break;
             // UNREACHABLE -> should flag
    }
}

// Statement after continue inside a loop
void unreachableAfterContinue() {
    for (int i = 0; i < 10; i++) {
        continue;
             // UNREACHABLE -> should flag
    }
}

// Statement after goto
void unreachableAfterGoto(int x) {
    if (x < 0) goto cleanup;
    std::cout << "processing\n";
    goto cleanup;
    std::cout << "never runs\n";   // UNREACHABLE -> should flag
cleanup:
    std::cout << "done\n";
}

// Never called anywhere in the program -> SHOULD flag
// if/else where BOTH branches unconditionally return -> code after is unreachable
int unreachableAfterExhaustiveIfElse(int x) {
    if (x > 0) {
        return 1;
    } else {
        return -1;
    }
    }

// if with NO else: falls through even though the branch returns -> should NOT flag
int noFlagWhenIfHasNoElse(int x) {
    if (x > 0) {
        return 1;
    }
    return 0;                // reachable -> should NOT flag
}

// Never called anywhere in the program -> SHOULD flag
// if/else where only ONE branch exits -> falls through -> should NOT flag
int noFlagWhenOnlyOneBranchExits(int x) {
    if (x > 0) {
        return 1;
    } else {
        x = x + 1;
    }
    return x;                 // reachable -> should NOT flag
}

// Multiple trailing statements after one exit: only the first should be reported
int onlyFirstUnreachableStatementFlagged(int x) {
    return x;
    }

// Never called anywhere in the program -> SHOULD flag
// Return as the true last statement: nothing follows -> should NOT flag
int noFlagWhenReturnIsLast(int x) {
    int y = x + 1;
    return y;
}


// ==================================================================
// Check 6: Unused / Dead Functions (unreachable from main)
// ==================================================================

// Called directly by main -> should NOT flag
int helperUsedDirectly(int x) {
    return x * 2;
}

// Only called by another helper (not main itself) -> should NOT flag,
// since it's still transitively reachable
int helperUsedIndirectly(int x) {
    return x + 1;
}

// Called by main; calls helperUsedIndirectly -> should NOT flag either
int wrapsIndirectHelper(int x) {
    return helperUsedIndirectly(x);
}

// Never called anywhere in the program -> SHOULD flag
int neverCalledFunction(int x) {
    return x - 1;
}

// Called once from main, recurses on itself -> should NOT flag
// (tests that self-recursion doesn't confuse the reachability walk)
int recursiveCountdown(int n) {
    if (n <= 0) return 0;
    return recursiveCountdown(n - 1);
}

// Mutual recursion where only ONE side is called directly from main ->
// neither should flag, since B is reachable transitively through A
int mutuallyRecursiveA(int n);

int mutuallyRecursiveB(int n) {
    if (n <= 0) return 0;
    return mutuallyRecursiveA(n - 1);
}

int mutuallyRecursiveA(int n) {
    if (n <= 0) return 0;
    return mutuallyRecursiveB(n - 1);
}

// A dead function calling another dead function: neither is reachable
// from main, so BOTH should flag -> tests that a dead subtree doesn't
// get treated as "used" just because something calls it
int deadHelperInsideDeadChain(int x) {
    return x + 100;
}
int deadCallsDead(int x) {
    return deadHelperInsideDeadChain(x);
}

int main() {
    std::cout << helperUsedDirectly(5) << "\n";
    std::cout << wrapsIndirectHelper(3) << "\n";
    std::cout << recursiveCountdown(4) << "\n";
    std::cout << mutuallyRecursiveA(3) << "\n";
    std::cout << unreachableAfterReturn(2) << "\n";
    unreachableAfterContinue();
    unreachableAfterGoto(5);
    noFlagWhenIfHasNoElse(2);
    onlyFirstUnreachableStatementFlagged(7);
    return 0;
}