Some questions to answer in your PLAN.md:

* **What option did you choose?**

**Option 3**: Develop a “smelly code” detector and refactor tool.

* **What exactly will your software do? (For example, if you chose Option 3, what language will you analyze? Which smells will you detect?)**  
  Given an input file (source code) read through to find code smells, execute to find current runtime then output version with no code smells and new runtime to the user. C++ will be the target language to analyze because of the wider opportunity for smelly code detection:


Smelly code to check: 

* Poor variable names  
* Redundant code  
* memory leaks  
* commenting  
* Repeated code (turn into function)  
    
    
* **What language(s) will you use?**  
  We have our eyes set on writing the project in C++. We have chosen C++ because of its high performance and our familiarity with it.  
    
* **What do you hope to accomplish in the remaining 2 hours?**  
  We aim to have a general idea on what we want our project to be. This includes the tools we want to use, visuals for the detector, goals for each sprint, and prioritizing features to implement.

This is just a tentative plan. You can change your mind later, regarding the feature set and tech stack. Commit to the repository by 3:00.   
Language: We have our eyes set on writing the project in C++ for maximum performance.  
   
Tools:   
Smelly code to check: 

* Poor variable names  
* Redundant code  
* memory leaks  
* commenting  
* Repeated code (turn into function)  
* infinite loops  
* 

Input:  
C++ source code 

Output:

* Line(s) of code to be refactored  
* Reasoning for refactoring  
* Comparison between old and new code (runtime, etc)

Sprint 1 plan:

* Read an input file  
* check for commenting (Aelynn)  
* check for frequently repeated code (Stuart)