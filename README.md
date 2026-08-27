# SmellyCodeDetector

> Important: This tool is intentionally heuristic-based and does not fully model every C++ edge case.
> The detector is designed for practical static-analysis warnings, not complete semantic understanding of all C++ programs.

A C++ static analysis tool that reads one or more source files, parses them once with tree-sitter,
and reports code smells as terminal warnings. The project is organized around a shared syntax tree,
so each detector reuses the same parsed AST rather than re-reading the file or regex-matching raw text.

The current implementation includes a full detector framework, a shared warning model, and safe
fix generation for several checks. The website in [index.html](index.html) also documents the
system architecture, major modules, diagrams, and current project status.

## Detector Limitations

- Some checks are pattern-based rather than full semantic analysis.
- Call-graph and points-to analysis is conservative and partial.
- Data-clump name suggestions depend on network access and a valid Gemini API key.
- Some warnings are informational only and do not include automatic fixes.
- The project focuses on the implemented smell types rather than every possible C++ code pattern.

## Team

- Stuart Arief
- Rebecca Jennings
- Jimmy Munoz
- Aelynn Xu

## Current Status

The project is currently in a working detector and documentation phase. The tool covers the
following smell checks:

- Poor Naming
- Redundant Code
- Dead/Unreachable Code
- Repeated Code
- Missing Comments
- Deeply Nested Conditionals
- Long Parameter Lists
- Data Clumps
- Redundant Inheritance
- Unfreed Memory

The project also includes a site-level project overview and architecture pages that describe the
core design, detector modules, explanations, and limit assumptions.

## Project Structure

- `main.cpp` � entry point and CLI flow
- `SmellyCodeDetector.*` � orchestrates all detectors and merges warnings
- `Parser.*` � reads/parses source files once
- `Detector.h` / `FixApplier.cpp` � common warning and fix infrastructure
- Detector implementations for each smell category
- `index.html` / `style.css` � project website and documentation

## Quick Start

### Prerequisites

- CMake 3.10+
- C++17 compiler
- Git
- Optional: Google Gemini API key for data-clump naming suggestions

### Build

```bash
git clone <repo-url>
cd project4
git submodule update --init --recursive
mkdir build
cd build
cmake .. -DBUILD_SHARED_LIBS=OFF
cmake --build .
```

### Run

From the project root:

```bash
./build/Debug/project4.exe
```

or from the build directory:

```bash
cd build/Debug
./project4.exe
```

The program will prompt for source files to analyze, then print warnings and offer fix application
for any check that supports automatic fixes.

## Build Notes

- The project uses tree-sitter and the C++ grammar.
- The repo includes vendored submodules under `tree-sitter/` and `tree-sitter-cpp/`.
- Data-clump naming suggestions require a `.env` file in the project root with:

```text
GOOGLE_GEMINI_API_KEY=your-api-key-here
```

## Detection Coverage

### Implemented checks

- Poor naming heuristics for variable, parameter, and function names
- Dead/unused variable detection with shadowing awareness
- Redundant boolean comparisons
- Redundant if/else boolean-return folding
- Chained if-return detection
- Unreachable code detection
- Unused functions via call-graph reachability checks
- Repeated statement block detection
- Missing comment detection for functions
- Deeply nested if detection
- Long parameter list detection
- Data clump detection with naming suggestions
- Redundant inheritance detection
- Memory leak detection for tracked allocations

### Auto-fix support

The following warnings currently support fix generation:

- Unused/dead variables
- Redundant boolean comparisons
- Redundant if/else boolean returns
- Unreachable code

Other checks still report warnings and/or suggestions without rewriting the file automatically.

## Website and Documentation

The project website is the current source for the detailed architecture and detector breakdown.
It contains the most up-to-date overview of the modules, diagrams, algorithms, and project timeline.
The README is kept as a concise developer-facing summary and setup guide.

## License and Project Context

This project was developed for CS179K as a software engineering and static analysis exercise.
The implementation intentionally focuses on practical detection patterns and system architecture over
full C++ language completeness.
