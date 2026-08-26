# SmellyCodeDetector

A C++ static analysis tool that scans one or more C++ source files and detects common code
smells, printing warning messages straight to the terminal. Each file is parsed once and
the resulting syntax tree is shared by all detectors. Parsing is powered by
[tree-sitter](https://tree-sitter.github.io/tree-sitter/), so every detector works directly
off the source's syntax tree rather than pattern-matching raw text.

> **Scope note:** due to complexity and limited time, this tool does not support every
> possible C++ use case. It focuses on detecting the code smells described below.

## Known Limitations

Quick early notice on each detector's blind spots — see the full documentation for details.

### RedundantCodeChecker
Only catches the specific patterns it's built for (e.g. `x == true`/`!= false`, an if/else
returning literal booleans, unconditional chained ifs) — logically equivalent code written
differently may not be flagged. Usage tracking can also misflag variables only touched through
aliasing or macros.

### DeadCodeChecker
Unreachable-code detection doesn't account for `throw` or infinite loops as exit paths. Unused-
function reachability is limited to the current file and can still miss behavior hidden behind
unsupported call patterns, despite the points-to analysis for recognized function and method
calls. The check is skipped if there is no `main()`.

### PoorNameChecker / NameAnalyzer / FunctionAnalyzer
Uses naming heuristics rather than semantic understanding. Short names, generic verbs, and
placeholder names can be intentional, while names hidden behind macros or unusual declarators
may not be analyzed as expected.

### RepeatedCodeChecker
Only compares repeated statement runs inside the same block. It reports structurally equal AST
subtrees, but does not identify semantically equivalent code that uses different operations or
different control flow.

### CommentChecker
Checks for the presence of comments, not whether a comment is accurate or useful. It operates
on comments recognized by tree-sitter and only evaluates function definitions in the current
translation unit.

### DeepIfDetector
Flags nesting deeper than `MAX_DEPTH` (currently 3). Deep nesting can be intentional, and the
detector does not assess whether a flatter design would actually be clearer.

### LongParamList
Flags functions with more than four parameters. The threshold is fixed and the detector does
not determine whether the parameters are genuinely related or whether grouping them improves
the API.

### DataClumpDetector
Reports recurring groups when the implementation finds a frequent triple of variables. It does
not enumerate every possible repeated subset, and its call-site analysis currently collects
identifier arguments rather than arbitrary expressions. Struct/class name suggestions require
network access and a configured Gemini API key.

### InheritanceChecker
Refactoring output removes the redundant inheritance clause; it does not redesign the class.Assignability analysis does not fully resolve ambiguity from parenthesis-style construction (for example, `Animal a(d)` can be mistaken for a function declaration).

### MemoryChecker
Tracks direct `new`, `malloc`, and `calloc` allocations by variable name and checks return paths.
It recognizes `delete`, `free()`, and custom functions that visibly deallocate a parameter, but
does not provide complete ownership, alias, interprocedural, or exception-path analysis.

## Table of Contents

- [Team Members](#team-members)
- [Features](#features)
  - [Poor Naming](#poor-naming)
  - [Redundant Code](#redundant-code)
  - [Repeated Code](#repeated-code)
  - [Missing Comments](#missing-comments)
  - [Deeply Nested Conditionals](#deeply-nested-conditionals)
  - [Data Clumps](#data-clumps)
  - [Inheritance](#inheritance)
  - [Memory Leaks](#memory-leaks)
  - [Dead Code Blocks](#dead-code-blocks)

- [Warning Output Format](#warning-output-format)
- [Refactor Suggestion Format](#refactor-suggestion-format)
- [Setup](#setup)
- [Building](#building)
- [Running](#running)
- [Testing](#testing)
- [Architecture](#architecture)
- [Planned Features (Not Yet Implemented)](#planned-features-not-yet-implemented)

## Team Members

- Stuart Arief
- Rebecca Jennings
- Jimmy Munoz
- Aelynn Xu

## Features

Each feature below is implemented as its own `Detector` subclass, walking the shared
tree-sitter syntax tree for each input file. See [Architecture](#architecture) for how they
fit together.

### Poor Naming

Flags ambiguous or unhelpful identifier names — variables, function names, and function
parameters — using a length heuristic (with exceptions for common loop counters like `i`,
`j`, `k`) plus regex checks for missing boolean prefixes, generic function verbs, and
placeholder names like `temp` or `data`.

```cpp
int main() {
  int t = 0;
  for (int i = 0; i < 5; i++) {
    t++;
  }
  std::cout << "Looped " << t << " times";
  return 0;
}
```

```
Warning: [Poor Naming]. Poor identifier name detected: 't'. Consider using a more descriptive name.(line 2)
```

### Redundant Code

Covers four related checks in a single traversal:

**Dead/unused variables** — declared but never referenced again within their own scope
(shadowing-aware, so a variable re-declared in a nested block doesn't count as a use of the
outer one).

```cpp
int calculateCost(int itemPrice, int tax) {
    int processFee = 5;               // never used
    int total = itemPrice + tax;
    return total;
}
```

```
Warning: [Redundant dead/unused variable]. The variable "processFee" is declared but never used.(line 2)
```

**Redundant boolean comparisons** — `x == true`, `x != false`, and similar, suggested as
their simplified form.

```cpp
if (isValid == true) {
    return totalCost;
}
```

```
Warning: [Redundant boolean comparison]. "isValid == true" can be simplified to "isValid".(line 1)
```

**Redundant if/else returning boolean literals** — branches that return opposite boolean
literals can collapse into the condition itself.

```cpp
bool caseA(bool cond) {
    if (cond) return true;
    else return false;
}
```

```
Warning: [Redundant-If-Else Return Boolean]. Can be simplified to "return cond;".(line 2)
```

**Chained if-statements that should be if/else if/else** — consecutive sibling `if`s that
each unconditionally return, with no `else` between them.

```cpp
std::string roleAssign(int number) {
    if (number < 10) { return "volunteer"; }
    if (number >= 10 && number < 20) { return "committee"; }
    if (number >= 20) { return "leader"; }
}
```

```
Warning: [Redundant Chained-If Statement]. 3 separate if-statements each return unconditionally; consider an if/else if/else chain instead.(line 2)
```

### Repeated Code

Detects runs of consecutive statements repeated elsewhere within the same block. Each
statement is structurally hashed so identical logic collides to the same key regardless of
exact formatting; a sliding window (largest first) then finds repeated, non-overlapping
runs, with every hash match confirmed by a full structural comparison to rule out
collisions.

```cpp
int scale(int x, int y) {
    int z = 0;
    z = x + y;
    y = x + z;
    x = y + z;
    z = x + y;
    y = x + z;
    x = y + z;
    return x;
}
```

```
Warning: [repeated-code]. Repeated code detected in function: scale
In line(s) 3 to 5
In line(s) 6 to 8
Repeated code:
z = x + y;
y = x + z;
x = y + z;
(line 3)
```

### Missing Comments

Flags any function with no preceding comment, no interior/inline comment, and no trailing
comment on the same line as its closing brace.

```cpp
int main() {
    int x = exFunction();
}

int exFunction() {
    return 1;
}
```

```
Warning: [Comments]. Function has no comments.(line 5)
```

### Deeply Nested Conditionals

Tracks `if`-statement nesting depth as it recurses and flags any `if` whose depth exceeds a
fixed maximum of 3, so overly nested conditional logic gets surfaced without a separate pass to
compute depth.

```cpp
void classify(int a, int b, int c, int d) {
    if (a > 0) {
        if (b > 0) {
            if (c > 0) {
                if (d > 0) {   // exceeds MAX_DEPTH
                    // ...
                }
            }
        }
    }
}
```

```
Warning: [deep-if]. Deeply nested if statement(line 5)
```

### Data Clumps

Tracks function lines that share the same set of variables and recommends
grouping these sets into a class or structure rather than writing each 
field individually.

The implementation looks for recurring variable groups in function parameters, call arguments,
and binary expressions. It reports groups that recur more than twice and currently promotes
frequent pairs into candidate triples before producing a warning.


```cpp
void getStreetNumber(std::string streetName, std::string city, std::string state, std::string zip) {
	std::cout << streetName << std::endl;
    std::string firstName;
    std::string lastName;
    std::string middleName;

    std::cout << firstName << " " << middleName << " " << lastName << std::endl;
}

void someFunction() {
    std::string streetName;
    std::string city;
    std::string state;
    std::string first;
    std::string second;
    std::string third;

    getStreetNumber(streetName, city, state, first, second);
    getStreetNumber(streetName, city, state, second, third);
    getStreetNumber(streetName, city, state, third, first);
}
```

```
Warning: Warning: [Data Clump]. The following lines share the same set of variables: 1, 11 Variables: city, state, streetName. Consider converting this to a struct or class with the name: Address.(line 7)
```

### Memory Leaks

Tracks a list of unfreed memory allocation locations and variable names found.

```cpp
int basicTest(){
    int* exampleptr = new int;
    return 2;
}

void multipleFunctionCallPartialTest(){
    int* exampleptr = new int;
    int* exampleptrTwo = new int;
    partialDeleterFunction(exampleptr, exampleptrTwo);
    return;
}

void partialDeleterFunction(int* one, int* two){
    delete one;
}
```

```
Warning: [memory-leak]. Unreleased memory allocated at line 2 does not reach a destructor.(line 3)
Warning: [memory-leak]. Unreleased memory allocated at line 8 does not reach a destructor.(line 10)
```

### Inheritance

Determines if classes with inheritance are redundant or not being used. It checks in-class base
usage, external access to base-only members, and object slicing through assignment or
pass-by-value calls.

```cpp
class Base {
public:
    virtual void greet();
    void helper();
    int baseField;
};

class UsesBaseConstructor : public Base {
public:
    UsesBaseConstructor(int x) : Base(), memberField(x) {}
    int memberField;
};
```

```
Warning: [unused-inheritance]. Class 'Widget' inherits from 'Base' but shows no evidence of using it. Consider removing the inheritance.(line 7)
```

### Dead Code Blocks

Covers two related checks in a single traversal:

**Unreachable code** — any statement that appears after a previous statement in the same
block has already unconditionally exited via return, break, continue, or goto (an
if/else only counts as exiting when both branches do).

```cpp
int getStatus(int code) {
    return code;
    std::cout << "logging: " << code << "\n";   // never runs
}
```

```
Warning: [unreachable-code]. This code can never execute because a previous statement in this block always exits via return/break/continue/goto.(line 3)
```

**Unused functions** — free functions and recognized class methods defined in the file but
never reachable from `main`, whether directly or transitively through other calls. The
reachability pass includes a points-to analysis for recognized function and method call paths;
self-recursion and mutual recursion are handled without repeatedly visiting the same function.

```cpp
int square(int x) {
    return x * x;
}

int cube(int x) {
    return x * x * x;
}

int main() {
    std::cout << square(4) << "\n";
    return 0;
}
```

```
Warning: [unused-function]. Function "cube" is never called from main (directly or indirectly) and is dead code.(line 5)
```

```cpp
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
```

```
Warning: [unused-function]. Function "Cat::makeSound" is never called from main (directly or indirectly) and is dead code.(line 11)
```

## Warning Output Format

Every detector returns its findings as a `std::vector<Warning>` instead of printing
anything itself. `SmellyCodeDetector::runDetectors()` merges every detector's results,
sorts them by line number (ties broken by category), and prints one line per warning:

```
Warning: [<category>]. <message>(line <line>)
```

It then reports the total number of warnings found in the file. When multiple files are given,
each file gets its own report and the program prints aggregate smell and processed-file totals
at the end.

## Refactor Suggestion Format

Warnings print directly to the terminal, one per detected smell, with the offending line number in the original source file. You'll then be prompted whether to apply automatic fixes:

`Some detected smells in "filename.cpp" have automatic fixes available. Apply them? (y/n): y`

`Fixed file written to: filename.cpp.fixed.cpp`

The fix prompt appears independently for each successfully opened file. Choosing "y" writes a new file by appending .fixed.cpp to the original path, leaving the original untouched. Choosing "n" skips that file.

Currently the features: Unused / Dead Variable, Redundant Boolean Comparison, Redundant If/Else Boolean Return, and Unreachable Code are auto-fixable and generate a new file. The other detectors still output warnings or suggest fixes in their warning output, but do not directly edit/genereate in the new file.

After all files finish, the program prints the total smell count and total number of processed files. Missing or unreadable paths are reported and skipped; the run continues with the remaining files.

## Setup

### Prerequisites

- **CMake** (3.10+) — [cmake.org/download](https://cmake.org/download/), or
  `winget install Kitware.CMake` on Windows. Confirm with `cmake --version`.
- **A C++17 compiler** — on Windows, Visual Studio 2022 (Community is free) with the
  "Desktop development with C++" workload, which provides MSVC and the generator CMake
  targets.
- **Git** — needed to pull in the tree-sitter runtime and grammar, which are vendored as
  submodules under `tree-sitter/` and `tree-sitter-cpp/`.
- **Internet access** — required during CMake configuration if `nlohmann/json` must be
    downloaded through `FetchContent`. The project builds its vendored libcurl dependency
    automatically; no separate curl installation is required.

### Getting the Code

```bash
git clone <repo-url>
cd project4
git submodule update --init --recursive
```

The submodule step is required — without it, `tree-sitter/` and `tree-sitter-cpp/` will be
empty and the CMake configure step will fail with a "not an existing directory" error.

### Gemini API setup

Data-clump detection uses Google Gemini to suggest a struct/class name. Create a `.env` file in
the project root and add your key without quotes:

```text
GOOGLE_GEMINI_API_KEY=your-api-key-here
```

Keep `.env` private; it is ignored by Git. The program searches for this file from the project
root and from the usual `build/Debug` launch directory. A data-clump run requires network access
to the Gemini API and uses a 30-second curl timeout. HTTP or transport failures use the fallback
name `DefaultName`; a missing key stops name generation with an error.

## Building

```bash
mkdir build
cd build
cmake .. -DBUILD_SHARED_LIBS=OFF
cmake --build .
```

This builds the tree-sitter runtime, the C++ grammar, vendored libcurl, and `project4` itself. On Windows
with the Visual Studio generator, the executable lands in a `Debug` subfolder.

## Running

```bash
cd Debug
./project4.exe
```

The program prompts for one or more whitespace-separated file names to analyze:

```
Enter the name(s) of the file(s) to read: myFile.cpp other.cpp
```

Paths are resolved relative to the directory you launch the executable from — since the
build places the `.exe` in `build/Debug`, either give a relative path back up to your
source files (e.g. `../../myFile.cpp`), or run the executable from the project root instead:

```bash
cd project4
./build/Debug/project4.exe
```

Warnings print directly to the terminal in the [format described above](#warning-output-format),
one per detected smell, with the offending line number in the original source file.

If a file has fixable warnings, the program asks whether to apply them for that file. A `y`
response writes a new file by appending `.fixed.cpp` to the original path (for example,
`myFile.cpp.fixed.cpp`) and leaves the source file unchanged. Unreadable paths are reported and
skipped while other input files continue processing.

Data-clump name generation additionally expects `GOOGLE_GEMINI_API_KEY` in `../../.env` relative
to the executable, with a fallback lookup at `../env`. Requests use libcurl, have a 30-second
timeout, and fall back to `DefaultName` for HTTP or transport failures. Missing credentials raise
an error when a name is requested.

## Testing

The repository currently provides detector-specific C++ fixture files rather than a separate
automated unit-test or CTest harness. Fixtures include `redundantCodeChecker/redundantTest.cpp`,
`deadCodeChecker/deadCodeTest.cpp`, `deadCodeChecker/pointsToTest.cpp`, `poorNameTest.cpp`,
`commentTest.cpp`, `repeatedTest.cpp`, `ifStatementTest.cpp`, `longParamTest.cpp`,
`DataClumpTest.cpp`, `inheritanceTest.cpp`, `memoryTest.cpp`, and `cleanTest.cpp`.

Build the target, run it from the project root, and enter fixture paths at the prompt:

```bash
cmake --build build
./build/Debug/project4.exe
```

Expected warnings are currently checked from terminal output. There is no committed script that
automatically asserts warning categories, line numbers, or generated fixed-file contents.

## Architecture

- **`Parser`** reads the input file once and builds a tree-sitter `TSTree`, returning a
  `ParsedSource { source, tree }` shared by every detector — the file is parsed exactly once
  no matter how many checks run against it.
- **`Warning`** is the shared result type every detector returns: a `line`, a `category`
  label (e.g. `"Poor Naming"`, `"deep-if"`), and a human-readable `message`.
- **`Detector`** is the abstract base every checker implements, exposing
  `analyzeSource(const ParsedSource&) -> std::vector<Warning>`. Detectors don't print
  anything themselves — they just collect and return warnings.
- **`SmellyCodeDetector`** owns the `Parser` and one instance of each detector
  (`PoorNameChecker`, `RedundantCodeChecker`, `RepeatedCodeChecker`, `CommentChecker`,
  `DeepIfDetector`, `InheritanceChecker`,`LongParamList`, `MemoryChecker`, `DeadCodeChecker`, 
  `DataClumpDetector`). `runDetectors()` runs `analyzeSource()` on all of them, merges the
  results, sorts by line (then category), prints the report, and returns the total warning
  count.
- Each detector walks the shared `TSTree` directly via a recursive `visitNode`, matching on
  real AST node types (e.g. `binary_expression`, `if_statement`, `function_definition`)
  rather than parsing raw text.
- `PoorNameChecker` delegates its actual name-quality checks to two small helper classes,
  `NameAnalyzer` (naming heuristics) and `FunctionAnalyzer` (applies those heuristics to
  function names and parameters).

A full breakdown of every file, class, and function signature is documented on the
project's [GitHub Pages site](https://ucr-cs-179k-summer-2026.github.io/project4/).

## Planned Features (Not Yet Implemented)

The following was part of the original project scope but is not currently implemented by
any detector in this repository. Listed here so the gap is visible rather than silently
dropped.

The current implementation is intentionally limited to the detector categories listed above.
Potential future work includes a regression-test harness, benchmark measurements, broader
cross-file analysis, and additional C++ control-flow and ownership cases.