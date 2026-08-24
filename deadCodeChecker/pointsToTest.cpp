#include <iostream>
#include <string>
#include <cstdlib>
// points-to-test-cases.cpp
//
// Test cases for the DeadCodeChecker points-to extension. Each case
// documents, inline, what SHOULD be flagged "Unused/Dead Function" and
// what SHOULD NOT, with the reasoning tied to the design decisions made
// in PointsToAnalyzer (unused-pointer seeding, conservative fallback on
// empty points-to sets, flow-insensitive union at merges).
//
// Suggest running these as separate files/compilation units through your
// detector rather than one giant file -- keeps the DFS-from-main results
// easy to eyeball per case, and avoids name collisions between cases
// (Case 2 and 5 both use "Dog"/"Cat").

// =============================================================================
// CASE 1: Original example -- the baseline points-to win over CHA/RTA
// =============================================================================
//
// EXPECTED FLAGGED (dead):
//   - Cat::makeSound
//     Reasoning: `c` in main and `f` in helper are declared but never
//     referenced again anywhere -- collectFromFunction's sub-pass A never
//     puts them in `referenced`, so they're never seeded. No Cat type
//     ever enters any points-to set in the whole program, so
//     resolveVirtualCall on `y`/makeSound only ever resolves Dog.
//
// EXPECTED NOT FLAGGED (live):
//   - Dog::makeSound
//     Reasoning: d (main) -> x (helper, via call arg) -> y (helper, via
//     one ternary arm) -> a (makeSounds, via call arg). e (helper) is
//     Dog*, so the other ternary arm also contributes Dog. points-to(y)
//     = {Dog} exactly, resolveVirtualCall(a, "makeSound") = {"Dog::makeSound"}.
//   - makeSounds, helper, main
//     Reasoning: main -> helper -> makeSounds is a plain reachable chain
//     of free-function calls, unaffected by the points-to work at all.
//   - Animal::makeSound is not a function_definition (no body), so it
//     never enters `functions` in the first place -- nothing to flag.

class Animal {
public:
    virtual void makeSound();
};
class Dog : public Animal {
public:
    void makeSound() { std::cout << "woof"; }
};
class Cat : public Animal {
public:
    void makeSound() { std::cout << "meow"; }
};
void makeSounds(Animal *a) {
    a->makeSound();
    a->makeSound();
}
void helper(Animal *x) {
    Dog *e;
    Cat *f;
    Animal *y = std::rand() ? x : e;
    makeSounds(y);
}
void main() {
    Dog *d;
    Cat *c;
    helper(d);
}


// =============================================================================
// CASE 2: Both subtypes actually flow -- confirms the analysis doesn't
// over-prune when more than one concrete type is genuinely live
// =============================================================================
//
// EXPECTED FLAGGED (dead):
//   - (nothing -- this case exists to prove the analysis isn't
//     accidentally always picking one branch)
//
// EXPECTED NOT FLAGGED (live):
//   - Dog::makeSound AND Cat::makeSound
//     Reasoning: unlike Case 1, `pet2` is a *used* Cat pointer (passed
//     into announce as an argument, which counts as a reference in
//     sub-pass A), so it gets seeded with {Cat}. Both `pet1` and `pet2`
//     flow into `p` across the two announce() calls; points-to(p) unions
//     to {Dog, Cat} (flow-insensitive -- both calls share the same VarId
//     for `p` since this analysis doesn't distinguish call sites).
//     resolveVirtualCall(p, "makeSound") = {"Dog::makeSound", "Cat::makeSound"}.
//   - announce, main

class Animal2 {
public:
    virtual void makeSound();
};
class Dog2 : public Animal2 {
public:
    void makeSound() { std::cout << "woof"; }
};
class Cat2 : public Animal2 {
public:
    void makeSound() { std::cout << "meow"; }
};
void announce(Animal2 *p) {
    p->makeSound();
}
void main2() {
    Dog2 *pet1;
    Cat2 *pet2;
    announce(pet1);
    announce(pet2);
}


// =============================================================================
// CASE 3: Dead function containing a virtual call -- confirms an
// unreached function's callees don't get spuriously marked reachable,
// AND that an override reachable ONLY from dead code is itself flagged
// =============================================================================
//
// EXPECTED FLAGGED (dead):
//   - unreachedCaller
//     Reasoning: never called from main, directly or indirectly -- plain
//     DFS-reachability, unrelated to points-to.
//   - Bird::makeSound
//     Reasoning: the ONLY call site that could ever resolve to
//     Bird::makeSound is inside unreachedCaller, which is itself
//     unreachable from main. The DFS never visits unreachedCaller, so it
//     never visits any edge unreachedCaller's callGraph entry contains --
//     Bird::makeSound is correctly never added to `reachable`, regardless
//     of what its own points-to resolution would have produced.
//
// EXPECTED NOT FLAGGED (live):
//   - Sparrow::makeSound
//     Reasoning: reached via the separate, actually-called path from main.
//   - main

class Animal3 {
public:
    virtual void makeSound();
};
class Bird : public Animal3 {
public:
    void makeSound() { std::cout << "tweet"; }
};
class Sparrow : public Animal3 {
public:
    void makeSound() { std::cout << "chirp"; }
};
void unreachedCaller() {
    Bird *b;
    Animal3 *a = b;
    a->makeSound();
}
void reachedCaller() {
    Sparrow *s;
    Animal3 *a = s;
    a->makeSound();
}
void main3() {
    reachedCaller();
}


// =============================================================================
// CASE 4: Empty points-to set -- exercises the conservative fallback
// =============================================================================
//
// EXPECTED FLAGGED (dead):
//   - (nothing among the overrides -- that's the point of this case)
//
// EXPECTED NOT FLAGGED (live):
//   - Wolf::howl AND Fox::howl
//     Reasoning: `w` is a parameter with no seed source ever tracked into
//     it in this simplified analysis (nothing calls mystery(), so no
//     argument-flow edge ever reaches its parameter `w`; it's also never
//     locally assigned from a declared concrete type). points-to(w) is
//     empty at solve time. Per the documented design choice in
//     PointsToAnalyzer::resolveVirtualCall, an empty points-to set falls
//     back to "every known override of the method is reachable" rather
//     than "none are" -- so BOTH Wolf::howl and Fox::howl are marked
//     live even though the analysis genuinely doesn't know which (or
//     whether either) actually executes. This is intentionally the
//     fail-toward-false-negatives behavior discussed earlier: better to
//     under-flag here than wrongly call a possibly-live method dead.
//   - mystery, main
//     (mystery is unreachable from main and would normally be flagged
//     dead on its own -- see Case 3's pattern -- but is included here
//     unreached on purpose to isolate the empty-points-to-set behavior
//     for its *parameter*, not to re-test reachability. If you want a
//     clean version of this case, call mystery(nullptr) from main so the
//     function itself doesn't also show up flagged for unrelated reasons.)

class Animal4 {
public:
    virtual void howl();
};
class Wolf : public Animal4 {
public:
    void howl() { std::cout << "awoo"; }
};
class Fox : public Animal4 {
public:
    void howl() { std::cout << "ring-ding-ding"; }
};
void mystery(Animal4 *w) {
    w->howl();
}
void main4() {
    mystery(nullptr);
}


// =============================================================================
// CASE 5: Plain (non-virtual) unused function + genuinely unused pointers
// -- baseline regression check, confirms Check 6's original behavior for
// free functions is untouched by the points-to changes
// =============================================================================
//
// EXPECTED FLAGGED (dead):
//   - unusedHelper
//     Reasoning: plain free function, never called from main -- exactly
//     the original (pre-points-to) behavior, unaffected by any of this.
//
// EXPECTED NOT FLAGGED (live):
//   - usedHelper, main5
//   - (no warning about `ghost` itself -- it's a plain unused *variable*,
//     which is a separate check (Check 1 in RedundantCodeChecker per your
//     project layout), not Check 6's concern. It should also correctly
//     NOT seed anything in the points-to graph, but there's no virtual
//     call anywhere in this case to demonstrate that with -- included
//     mainly as a sanity check that non-virtual code paths aren't
//     affected by any of the new machinery.)

void usedHelper() {
    std::cout << "used";
}
void unusedHelper() {
    std::cout << "never called";
}
void main5() {
    int ghost;
    usedHelper();
}