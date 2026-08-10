## Project Summary:

Smelly code detector written in C++. The program takes a C++ source code file as input and uses tools such as C++’s regex library to read it and detect smelly code and outputs warning messages on the terminal based on that comparison. Note: due to complexity and limited time, the program will not support all use cases of C++ and will instead focus on detecting smelly code that meets criteria that will be more clearly described in the features section of this document.

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

**Redundant Initialization**
```
main() {
	int totalScore = 0;
	totalScore = calculateFinalScore();
}
```

Output: Warning: Redundant initialization. Initialize with “int totalScore = calculateFinalScore();” 



