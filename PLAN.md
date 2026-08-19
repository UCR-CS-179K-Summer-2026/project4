# Week 1 Sprint Goals

Stuart Arief:

* Implement working ‘proof of concept’ for the entire detector.  
* Partially functional detection of ‘multiple lines of repeated code’ with basic base cases (If a block of code containing 3 lines of code is repeated in different parts of the input file, emit the warning message).

Rebecca Jennings:

* Implement a basic detector for the redundant code case of unused/dead code (variables).
* Test with C++ file to identify one unused/dead code in main() . 
* Test with C++ file to identify multiple unused/dead code in main() . 
* Test C++ file to identify multiple unused/dead code across multiple functions, conditional statements, etc.

Jimmy Munoz:

* Implement the basic functionality to identify a poor variable name. Will focus on loops for now.  
* Once identified, recommend a more sufficient variable name rather than an ambiguous one.
* Expand to more test cases in this scope. Find a way to analyze the code to recommend a more sufficient variable name when necessary.

Aelynn Xu:

* Implement reading through the input file and identifying functions with regex.
* Identify comments.
* If there is time, begin basic work on detecting if each function has at least one comment.

# Week 2 Sprint Goals

Stuart Arief:

* Implement functionality to allow for repeated blocks of code with same logic but different variable names to be detected.
* Modify main.cpp so that it could process multiple input files as opposed to just one.
* Update system architecture.
* Update test cases to cover more edge cases.

Rebecca Jennings:

* Modify redundantCodeChecker.cpp when we shift to new parser library.
* Modify redundantCodeChecker.cpp to identify a base case for redundant boolean logic.
* Implement extensive test cases for identifying unused/dead variables and redundant boolean logic.
* Update UML Diagram and documentation.

Jimmy Munoz:

* Use regex for finding poor variable names instead of relying on a map with every possible case.
* Add more test cases (function parameter names, boolean names, and abbreviations).
* Update parser to include function parameters in the DeclaredVariable array.

Aelynn Xu:

* Write function to strip strings from source code in parser.
* Modify parser so its functions are available for separate use in detectors.
* Update commentchecker using aforementioned function so it will not be tripped up by brackets in string literals.
* Update commentChecker so that it will not be tripped up by brackets in ‘’ chars.
* Update commentchecker so that it will not be tripped up by brackets in comments.

# Week 3 Sprint Goals

Stuart Arief:

* Fix repeatedCodeDetector.cpp not ignoring whitespaces by using TSNode instead of old regex functions.
* Transition to a new lint: Create a class that detects function headers that have unnecessarily long list of parameters.
* Transition to a new lint: Create a class that detects unnecessary links between two classes.
* Delete unused files in the project directory.
* Add comments to cpp files to improve readability.

Rebecca Jennings:

* Revise redundantTest.cpp to account for multiple edge cases and combine cases for each of the checks implemented.
* Create a Universal Test file to detect and output warnings for all our code detectors.
* Revised Dead/Unused Variable case to account for same variable names that are dead, despite being reinitialized in a different scope.
* Implement helper function to add our warnings to a vector and sort in ascending order.
* Implement logic to detect Dead/Unused Code Blocks.

Jimmy Munoz:

* Create a new lint to detect nested if-statements that are too long. Any nested if-statements longer than 3 will be flagged.
* Create a new lint that detects the length of each function. If a function is too long, about more than 50 - 100 lines, the function will be flagged.
* Create a new lint to find literal values that are not explained. If found, flag it and recommend the user to use a constant.

Aelynn Xu:

* List out and categorize cases for memory leaks.
* Implement memory leak detector to account for various cases.

 # Week 4 Sprint Sprint Goals

Stuart Arief:

* Create pseudocode flow chart for all of my implemented features 
* Handle an edge case for inheritanceChecker that accounts for subclasses that do not use anything from the base class but when an object of the subclass is made in a different function like main(), it uses the object to call a function from its base class, thus not being a candidate for detection.
* Add more comprehensive comments to all the features made, based on what I have written on the documentation.
* Handle an edge case of assignability. Both cases: one where we use the assignment operator “Animal a = Dog d;” and one where we use a function to assign it (this one is challenging).
* Basically the first edge case is for calling members of the baseClass in a different function. While the second case is assignment in a different function.

Rebecca Jennings:

* Create flow charts for redundantCodeChecker and deadCodeChecker.
* Modify Unreachable Code warning outputs to display the range lines of effected.
* Work on combining and documenting shared control flow analysis.
* Research point to analysis to expan DeadCodeBlocks - unused function feature to recognize pointers and references.

Jimmy Munoz:

* Create a flowchart for the detectors I created.
* Solve the edge case of a variable in a block of variable declarations that is not used with that block. Consider counting the amount of declarations instead of searching for groups.

Aelynn Xu:

* Edge cases for memory leak (interprocedural, etc).
* Shared functions from redundantCode.
* Documentation updates.





