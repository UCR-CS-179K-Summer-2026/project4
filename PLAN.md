# Project Plan Summary

The SmellyCodeDetector project was built as a multi-detector static analysis tool for C++ code smells. The overall plan was to create a shared parser and AST pipeline, then implement a set of detectors that each identify a different smell, unify their warnings, and give users safe refactoring suggestions when possible.

The project evolved from a basic proof-of-concept into a working tool with architecture documentation, detector flow diagrams, multi-file input support, and finishing touches for documentation, edge cases, and demo readiness.

## Sprint 1: Proof of Concept and Initial Detectors

### Stuart Arief
- Implement a working proof of concept for the entire detector.
- Build initial repeated-code detection for repeated blocks of 3+ lines.

### Rebecca Jennings
- Create the first unused/dead variable detector.
- Test detection with one unused variable in main().
- Test detection with multiple unused variables in main().
- Test detection across multiple functions and conditional blocks.

### Jimmy Munoz
- Build a basic poor-name detector focused on loop variables.
- Recommend more descriptive names when weak identifiers are found.
- Expand tests for poor naming detection and naming suggestions.

### Aelynn Xu
- Read the input file and identify functions using regex.
- Identify comment placement and usage.
- Start basic detection of whether each function has at least one comment.

## Sprint 2: Parser Improvements and Better Core Checks

### Stuart Arief
- Add repeated-code detection that handles same logic with different variable names.
- Update main.cpp to accept multiple input files.
- Improve the overall system architecture.
- Extend test coverage for more edge cases.

### Rebecca Jennings
- Update the redundant code checker after the parser transition.
- Add a base case for redundant boolean logic.
- Expand tests for unused variables and redundant boolean logic.
- Update UML diagrams and project documentation.

### Jimmy Munoz
- Replace map-based name matching with regex-based poor-name detection.
- Add more tests for function parameter names, boolean names, and abbreviations.
- Update the parser to include function parameters in the variable analysis.

### Aelynn Xu
- Add a function to strip strings from the source code in the parser.
- Expose parser functions for use across detectors.
- Update comment checking to ignore brackets in string literals.
- Update comment checking to ignore brackets in single quotes and comments.

## Sprint 3: More Detectors and Validation Work

### Stuart Arief
- Fix repeated-code detection by removing whitespace sensitivity using TSNode instead of regex.
- Create a long-parameter-list detector.
- Create a detector for unnecessary or excessive primitive data usage.
- Remove unused files.
- Add comments across cpp files to improve readability.

### Rebecca Jennings
- Revise test cases for multiple edge conditions.
- Create a universal test file for all detectors.
- Improve dead/unused-variable detection for variables reinitialized in different scopes.
- Add warning aggregation and sorting utilities.
- Implement dead/unused code block detection.

### Jimmy Munoz
- Create a deeply nested if detector with a threshold beyond 3 levels.
- Create a function-length detector for long functions.
- Create a literal-value detector that flags unexplained magic numbers and recommends constants.

### Aelynn Xu
- Categorize memory leak scenarios.
- Implement a memory leak detector for various memory allocation cases.

## Sprint 4: Current Implementation Expansion

### Stuart Arief
- Expand inheritance analysis to handle external base-member access, object slicing, assignment, and base-constructor usage.
- Add refactoring output for inheritance with original code, revised code, and changelog.
- Add detector flow diagrams and expanded implementation documentation.

### Rebecca Jennings
- Expand unreachable-code reporting to include affected line ranges.
- Add shadowing-aware unused-variable handling and call-graph unused-function detection.
- Implement points-to analysis for pointer and function-call relationships.
- Add fix application support for safe source rewrites.
- Add flow diagrams and update the website documentation.

### Jimmy Munoz
- Expand data-clump detection for recurring variable groups in parameters, calls, and expressions.
- Add API-backed naming suggestions for data clumps.
- Add flow diagrams for detector work.

### Aelynn Xu
- Expand memory analysis to check conditional paths and recognize delete, free, and custom deallocation functions.
- Add memory-leak flow diagrams and edge-case coverage.

## Sprint 5: Final Implementation and Demo Polish

### Stuart Arief
- Fix inheritance bugs related to derived-class object usage and non-virtual base members.
- Add more varied inheritance tests.
- Record demo video for final documentation.
- Add refactoring features for repeated-code detection and long parameter lists.

### Rebecca Jennings
- Add test cases for each code checker.
- Create a universal real-world test suite.
- Continue iterative website updates.
- Record demo video for final documentation.

### Jimmy Munoz
- Improve data-clump detection to identify groups of three or more variables.
- Add more variable-clump tests.
- Record demo video for final documentation.

### Aelynn Xu
- Solve pointer aliasing issues.
- Fix nested function support.
- Record demo video for final documentation.

## Overall Project Goal

The project’s main goal was to build a practical static analysis tool that catches common C++ code smells using tree-sitter AST traversal, shared detector architecture, and safe fix suggestions. The final site and documentation reflect the finished implementation and the iterative improvements made across these five sprint phases.

