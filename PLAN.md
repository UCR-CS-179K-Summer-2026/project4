# Week 1 Sprint Goals:

Stuart Arief:

* Implement working ‘proof of concept’ for the entire detector  
* Partially functional detection of ‘multiple lines of repeated code’ with basic base cases (If a block of code containing 3 lines of code is repeated in different parts of the input file, emit the warning message)

Rebecca Jennings:

* Implement a basic detector for the redundant code case of unused/dead code (variables)  
* Test with C++ file to identify one unused/dead code in main()  
* Test with C++ file to identify multiple unused/dead code in main()  
* Test C++ file to identify multiple unused/dead code across multiple functions, conditional statements, etc.

Jimmy Munoz:

* Implement the basic functionality to identify a poor variable name. Will focus on loops for now.  
* Once identified, recommend a more sufficient variable name rather than an ambiguous one  
* Expand to more test cases in this scope. Find a way to analyze the code to recommend a more sufficient variable name when necessary

Aelynn Xu:

* Implement reading through the input file and identifying functions with regex  
* Identify comments  
* If there is time, begin basic work on detecting if each function has at least one comment

# Week 2 Sprint Goals:

Stuart Arief:

* Implement functionality to allow for repeated blocks of code with same logic but different variable names to be detected
* Modify main.cpp so that it could process multiple input files as opposed to just one
* Update system architecture
* Update test cases to cover more edge cases

Rebecca Jennings:

* Implement extensive test cases for identifying unused/dead variables in a c++ file
* Modify redundantCodeChecker.cpp when we shift to new parser library
* Modify redundantCodeChecker.cpp to identify unused/dead functions in a c++
* Test with C++ file to identify multiple base case scenarios for unused/dead functions
* Create UML Diagram and update documentation

Jimmy Munoz:

* Use regex for finding poor variable names instead of relying on a map with every possible case
* Add more test cases (function parameter names, boolean names, and abbreviations)
* Update parser to include function parameters in the DeclaredVariable array

Aelynn Xu:

* Write function to strip strings from source code in parser
* Modify parser so its functions are available for separate use in detectors
* Update commentchecker using aforementioned function so it will not be tripped up by brackets in string literals
* Update commentChecker so that it will not be tripped up by brackets in ‘’ chars
* Update commentchecker so that it will not be tripped up by brackets in comments
