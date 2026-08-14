## Project Summary:

A C++ static analysis tool that scans a C++ source file and detects common code smells,
printing warning messages to the terminal. Parsing is powered by
[tree-sitter](https://tree-sitter.github.io/tree-sitter/), so detectors work directly off
the source's syntax tree rather than pattern-matching raw text.
 
> **Scope note:** due to complexity and limited time, this tool does not support every
> possible C++ use case. It focuses on detecting the code smells described below.

## Team Members:

- Stuart Arief
- Rebecca Jennings
- Jimmy Munoz
- Aelynn Xu

## Initial Features & Examples:

### Bad Variable Names
1. Bad variable names: This feature will detect a variable name that may be improved. In the example below, the variable t represents our counter variable. Instead of having an ambiguous identifier name such as t, recommend another variable name such as counter. 
Input:
```
int main(){
  int t = 0;
  for (int i = 0; i<5 ; i++){
  t++;
  }
  
  std::cout << “Looped” << t << “ times”;
  return 0;
}
```
Output: Warning: The allocated variable ‘int t’ has an ambiguous identifier.

### Repeated Code

2. Repeated code: This feature detects lines of code which are needlessly repeated throughout the input file. In the example below, the first lines of code initialize the variables with values of 0, which results in the same value of 0 for any mathematical operation performed using them. The program recognizes this and then emits a respective warning message to the terminal.
Input:
```
main(){
  int x = 0;
  int y = 0;
  int z = 0;
  z = x + y;
  y = x + z;
  x = y + z;
  z = x + y;
  y = x + z;
  x = y + z;
}
```
Output: Warning: Repeated code

### Memory Leak

3. Memory leak example: This feature will detect a dynamically allocated variable and determine whether it has been de-allocated properly once it is not used anymore. If it detects that it was never deleted, it will send a warning message about the issue.

Input:
```
int main() {
	int* number = new int;
	number = 5;
	std::cout << number * 3 << std::endl;
  return 0; 
}
```

Output: Warning: The allocated variable ‘number’ has not been deleted. Please add in the line ‘delete number;’ to de-allocate.

### Commenting

4. Commenting: For our code smell detector, we will create a function to detect if there is at least one comment per function. If there is time, these criteria may be refined (detecting if the comments are actually saying something instead of gibberish, for example)

Input:
```
int main() {
		int x = exFunction();
}

int exFunction(){
		return 1;
}
```
Output: Warning: Uncommented function exFunction

### Redundant Code

5. Redundant code: This feature will detect code that can be removed or refined without affecting its desired behavior, otherwise known as redundant code. Some cases include excessive conditional statements, dead/unused variables, redundant initialization of variables, and excessive boolean logic. There are variations of redundant code that may be expanded upon depending on the progress of the project.

**Dead/Unused Variables**

**User story:** As a developer, I want the tool to flag when I declare a variable but never use it, so I can clean up leftover code before committing and avoid confusion for anyone reading the function later.
```
int calculateCost(int itemPrice, int tax) {
	int processFee = 5;				// should -> variable is dead/unused
	int total = itemPrice + tax;
	return total;
}
```
Warning: Redundant dead/unused code. The variable "processFee" is declared but never used. (line 2)

**Redundant Boolean and If/Else Boolean Logic**

**User Story:** As a developer using the smelly code detector, I want the tool to flag comparisons like x == true or x == false, so that I can simplify my conditionals to x or !x and return x or return !x to keep my codebase more readable.

```
int main() {
	if(isValid == true) { 					// should flag -> simplifies to "isValid"
        int totalCost = processOrder(quantity, price);
        return totalCost;
    } 
    if(isValid == false) {					// should flag -> simplifies to "!isValid"
        return price;
    }
}
```

Output: Warning: Redundant boolean comparison. "isValid == true" can be simplified. (line 2)
Warning: Redundant boolean comparison. "isValid == false" can be simplified. (line 6)

```
bool caseA(bool cond) {
    if (cond) return true;
    else return false;          // should flag -> simplifies to "return cond;"
}
```

Output: Warning: Redundant if/else returning boolean literals. Can be simplified to "return cond;". (line 5)

**Redundant Conditional Statements**

**User story:** As a developer using the smelly code detector, I want the tool to flag conditonals like repeated if statements, so that I can simplify my conditionals to an if/else if/else chain to keep my codebase more readable.
```
std:: string roleAssign(int number, int time) {
    if (number < 10) { return "volunteer"; }
    if (number >= 10 && number < 20) { return "committee"; }
    if (number >= 20) { return "leader"; }

    if (time < 5) { return "junior"; }
    else if (time >= 5 && time < 10) { return "senior";} 
    else if (time >= 10) { return "expert"; }
    else { return "unknown"; }
}
```

Output: Warning: Redundant conditional statement. 3 separate if-statements each return unconditionally; consider an if/else if/else chain instead. (starting line 2)
---
 
## Setup
 
### Prerequisites
- **CMake** (3.10+) — [cmake.org/download](https://cmake.org/download/), or `winget install Kitware.CMake` on Windows. Confirm with `cmake --version`.
- **A C++17 compiler** — on Windows, Visual Studio 2022 (Community is free) with the
  "Desktop development with C++" workload, which provides MSVC and the generator CMake targets.
- **Git** — needed to pull in the tree-sitter runtime and grammar, which are vendored as
  submodules under `tree-sitter/` and `tree-sitter-cpp/`.
  
### Getting the code
```bash
git clone <repo-url>
cd project4
git submodule update --init --recursive
```
The submodule step is required — without it, `tree-sitter/` and `tree-sitter-cpp/` will be
empty and the CMake configure step will fail with a "not an existing directory" error.
 
---

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
 
The program prompts for one or more file names to analyze:
```
Enter the name(s) of the file(s) to read: myFile.cpp
```
 
Paths are resolved relative to the directory you launch the executable from — since the
build places the `.exe` in `build/Debug`, either give a relative path back up to your
source files (e.g. `../../myFile.cpp`), or run the executable from the project root instead:
```bash
cd project4
./build/Debug/project4.exe
```
 
Warnings print directly to the terminal, one per detected smell, with the offending line
number in the original source file.
 
---

## Architecture
 
- **`Parser`** reads the input file once and builds a tree-sitter `TSTree`, returning a
  `ParsedSource { source, tree }` shared by every detector — the file is parsed exactly once
  no matter how many checks run against it.
- **`Detector`** is the abstract base every checker implements, exposing
  `analyzeSource(const ParsedSource&)`.
- **`SmellyCodeDetector`** owns the `Parser` and one instance of each detector, running
  `analyzeSource()` on all of them and summing the total warning count.
- Each detector walks the shared `TSTree` directly via a recursive `visitNode`, matching
  on real AST node types (e.g. `binary_expression`, `if_statement`) rather than parsing
  raw text — this also means warning line numbers come straight from the tree
  (`ts_node_start_point`) instead of manually tracked offsets.



