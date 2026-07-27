# PLAN.md:

* **What option did you choose?**

**Option 3**: Develop a “smelly code” detector and refactor tool.

* **What exactly will your software do? (For example, if you chose Option 3, what language will you analyze? Which smells will you detect?)**

Given an input file (source code) read through to find code smells, execute to find current runtime then output version with no code smells and new runtime to the user. C++ will be the target language to analyze because of the wider opportunity for smelly code detection:

* **What language(s) will you use?**

We have our eyes set on writing the project in C++. We have chosen C++ because of its high performance and our familiarity with it.

* **What do you hope to accomplish in the remaining 2 hours?**

We aim to have a general idea on what we want our project to be. This includes the tools we want to use, visuals for the detector, goals for each sprint, and prioritizing features to implement.

This is just a tentative plan. You can change your mind later, regarding the feature set and tech stack. Commit to the repository by 3:00.

Smelly code to check:

* Poor variable names

* Redundant code

* memory leaks: check destructor is working, check that it gets called for every allocated pointer

* commenting: check that each function has a comment (hopefully explaining what the function does)

* Repeated code: note any repeated code sections 3 lines that could be changed into a function

Input:

* C++ source code

Output:

* Line(s) of code to be refactored

* Reasoning for refactoring

* Comparison between old and new code (runtime, etc)

Sprint 1 Plan:

* Read an input file

* check for commenting (Aelynn)

* check for frequently repeated code (Stuart)

* Checking for redundant code (Rebecca)

* Check for poor variable names (Jimmy)

Feedback:

* Convert C++ file into an intermediate representation to be analyzed to detect and refactor code smells

* Look into existing tools or libraries, but not one that covers all our material → use to parse the code and convert to IR

* Update to specify features our checker will support

* GOAL by 4:00pm: examples of code input (C++) and the output (error message or report to the user specifying) → concrete/tangible examples

* Decide if we are analyzing all of C++ or subset of C++

* Come with multiple examples and what we will report

Example outputs:

1. **Bad variable names**

**Input:**

```cpp
int main(){

int t = 0;

for (int i = 0; i<5 ; i++){

t++;

}

std::cout << “Looped” << t << “ times”;

return 0;

}
```

**Output:**

```text
Warning: The allocated variable ‘int t’ has an ambiguous identifier.
```

1. **Memory leak example**

Input:

```cpp
int main() {

int* number = new int;

number = 5;

std::cout << number * 3 << std::endl;

return 0; 

}
```

Output:

```text
Warning: The allocated variable ‘number’ has not been deleted. Please add in the line ‘delete number;’ to de-allocate.
```

1. **Commenting**

Input:

```cpp
int main() {

int x = exFunction();

}

int exFunction(){

return 1;

}
```

Output:

```text
Warning: Uncommented function exFunction
```

1. **Redundant Code**

**Conditional Statements**

```cpp
string numType(int number) {

if (number > 0) {

return “positive”;

}

if (number == 0)  {

return “zero”;

}

if (number < 0) {

return “negative”;

}

}
```

Output:

```text
Warning: Redundant conditional statement. Modify the function to use “if”, “if else”, and “else” for “number” conditions.
```

**Dead/Unused Code**

```cpp
int calculateCost(int itemPrice, int tax) {

int processFee = 5;

int total = itemPrice + tax;

return total;

}
```

Output:

```text
Warning: Redundant dead/unused code. “processFee” is declared but never used.
```

**Initialization**

```cpp
main() {

int totalScore = 0;

totalScore = calculateFinalScore();

}
```

Output:

```text
Warning: Redundant initialization. Initialize with “int totalScore = calculateFinalScore();” 
```

**Boolean**

```cpp
bool isEligible(int age) {

if (age >= 18) {

return true;

}

else {

return false;

}

}
```

Output:

```text
Warning: Redundant boolean logic. Modify the function to “return age >= 18;”.
```

```cpp
int main() {

bool isGameOver = checkStatus();

if (isGameOver == true) {

endGame():

}

}
```

Output:

```text
Warning: Redundant boolean logic. Modify the statement to “if (isGameOver)”.
```

1. **Repeated code**

Input:

```cpp
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

Output:

```text
Warning: Repeated code
```
