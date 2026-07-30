**Week 1 Sprint Goals:**

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

* implement reading through the input file and identifying functions with regex  
* identify comments  
* if there is time, begin basic work on detecting if each function has at least one comment

