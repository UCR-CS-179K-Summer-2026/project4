# SmellyCodeDetector

A C++ static analysis tool that scans a C++ source file and detects common code smells,
printing warning messages straight to the terminal. Parsing is powered by
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
function detection only tracks plain function calls (not member calls, namespaced calls, or
function pointers), is skipped if there's no `main()`, and is single-file only.

### PoorNameChecker / NameAnalyzer / FunctionAnalyzer
*(limitations TBD)*

### RepeatedCodeChecker
*(limitations TBD)*

### CommentChecker
*(limitations TBD)*

### DeepIfDetector
*(limitations TBD)*

### LongParamList
*(limitations TBD)*

### DataClumpDetector
*(limitations TBD)*

### InheritanceChecker
- offerRefactoring changelog is currently only handling removal of inheritance call, nothing else
- Assignability edge case does not handle ambiguity from parenthesis styled construction (e.g. Animal a(d)) which can be mistaken for a function instead of constructor call.\
### MemoryChecker
*(limitations TBD)*

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
- [Setup](#setup)
- [Building](#building)
- [Running](#running)
- [Architecture](#architecture)
- [Planned Features (Not Yet Implemented)](#planned-features-not-yet-implemented)

## Team Members

- Stuart Arief
- Rebecca Jennings
- Jimmy Munoz
- Aelynn Xu

## Features

Each feature below is implemented as its own `Detector` subclass, walking the shared
tree-sitter syntax tree once per file. See [Architecture](#architecture) for how they fit
together.

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
fixed maximum, so overly nested conditional logic gets surfaced without a separate pass to
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
```

```
Warning: [memory-leak]. Unreleased memory allocated at  at line 2 does not reach a destructor.(line 3)
```

### Inheritance

Determines if functions or other classes with inheritance are redudant or not being used.

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
Warning: [unused-inheritance]. Class 'UsesBaseConstructor' inherits from 'Base' but shows no using it (no qualified Base::member calls, no explicit base constructor call, no override/final specifier). Consider removing the inheritance.(line 7)
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
Warning: [Unreachable Code]. This code can never execute because a previous statement in this block always exits via return/break/continue/goto.(line 3)
```

**Unused functions** — functions defined in the file but never reachable from main,
whether directly or transitively through other calls (self-recursion and mutual recursion
are both handled correctly and won't cause a false flag on their own).

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
Warning: [Unused Function]. Function "cube" is never called from main (directly or indirectly) and is dead code.(line 5)
```

## Warning Output Format

Every detector returns its findings as a `std::vector<Warning>` instead of printing
anything itself. `SmellyCodeDetector::runDetectors()` merges every detector's results,
sorts them by line number (ties broken by category), and prints one line per warning:

```
Warning: [<category>]. <message>(line <line>)
```

It then reports the total number of warnings found in the file.

## Setup

### Prerequisites

- **CMake** (3.10+) — [cmake.org/download](https://cmake.org/download/), or
  `winget install Kitware.CMake` on Windows. Confirm with `cmake --version`.
- **A C++17 compiler** — on Windows, Visual Studio 2022 (Community is free) with the
  "Desktop development with C++" workload, which provides MSVC and the generator CMake
  targets.
- **Git** — needed to pull in the tree-sitter runtime and grammar, which are vendored as
  submodules under `tree-sitter/` and `tree-sitter-cpp/`.

### Getting the Code

```bash
git clone <repo-url>
cd project4
git submodule update --init --recursive
```

The submodule step is required — without it, `tree-sitter/` and `tree-sitter-cpp/` will be
empty and the CMake configure step will fail with a "not an existing directory" error.

## Building

```bash
mkdir build
cd build
cmake .. -DBUILD_SHARED_LIBS=OFF
cmake --build .
```

This builds the tree-sitter runtime, the C++ grammar, and `project4` itself. On Windows
with the Visual Studio generator, the executable lands in a `Debug` subfolder.

## Running

```bash
cd Debug
./project4.exe
```

The program prompts for a file name to analyze:

```
Enter the name of the file to read: myFile.cpp
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

**TBD**



