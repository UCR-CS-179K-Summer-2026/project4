# SmellyCodeDetector

A C++ static analysis tool that scans one or more source files, parses them once with tree-sitter,
and reports code smells directly in the terminal. The project is built around a shared AST so each
checker reuses the same parse result instead of re-reading or pattern-matching raw text.

> Scope note: this project is intentionally heuristic-based. It is designed to catch common code smells
> and support practical refactoring suggestions, but it does not attempt to fully model every C++ edge case.

## Current Limitations

- Some checks are pattern-based rather than full semantic analysis.
- Call-graph and points-to analysis is conservative and partial.
- Data-clump name suggestions depend on network access and a valid Gemini API key.
- Some warnings are informational only and do not include automatic fixes.
- The project focuses on the implemented smell types rather than every possible C++ code pattern.

## Table of Contents

- [Overview](#overview)
- [Highlights](#highlights)
- [Architecture](#architecture)
- [System Requirements](#system-requirements)
- [Setup](#setup)
- [Quick Start](#quick-start)
- [Testing](#testing)
- [Current Limitations](#current-limitations)
- [Team](#team)

## Overview

SmellyCodeDetector is a static analysis project for C++ code quality. It analyzes source files with a
shared tree-sitter parse tree and runs a set of independent detectors to identify common smells such as
poor names, dead variables, redundant logic, repeated code, memory leaks, long parameter lists, and
inheritance misuse.

The project also supports safe auto-fixes for some warnings, producing a copy of the file with a
`.fixed.cpp` suffix when the fix is straightforward and low-risk.

## Highlights

- Shared parser and AST pipeline across all detectors
- Multiple smell checks in one run
- Warning output with source line numbers
- Auto-fix support for selected warning types
- Website documentation with architecture diagrams and implementation notes

### Implemented detectors

- Poor Naming
- Redundant Code
- Dead / Unreachable Code
- Repeated Code
- Missing Comments
- Deeply Nested Conditionals
- Long Parameter Lists
- Data Clumps
- Redundant Inheritance
- Unfreed Memory

## Architecture

The system is organized around a small set of shared structures and a detector framework:

- `Parser` reads the input file once and builds the tree-sitter `TSTree`
- `ParsedSource` holds the original source plus the parse tree
- `Detector` is the abstract base class for each smell checker
- `Warning` stores the category, message, source line, and optional fix
- `SmellyCodeDetector` runs every detector, merges their warnings, sorts them, and prints the final report
- `FixApplier` applies safe edits back to a copy of the source when a fix is available

This design keeps the analysis consistent and avoids repeated parsing for each detector.

## System Requirements

- CMake 3.10+
- C++17 compiler
- Git
- Optional: Google Gemini API key for data-clump naming suggestions

## Setup

### Clone and initialize submodules

```bash
git clone <repo-url>
cd project4
git submodule update --init --recursive
```

### Build

```bash
mkdir build
cd build
cmake .. -DBUILD_SHARED_LIBS=OFF
cmake --build .
```

### Gemini setup (optional)

If you want data-clump naming suggestions to use the Gemini API, create a `.env` file in the project root:

```text
GOOGLE_GEMINI_API_KEY=your-api-key-here
```

## Quick Start

From the project root:

```bash
./build/Debug/project4.exe
```

Or from the build directory:

```bash
cd build/Debug
./project4.exe
```

The program will prompt for one or more source files to analyze. It prints warnings to the terminal and,
for files with safe auto-fixes, asks whether to write a `.fixed.cpp` version.

## Testing

The project uses fixture-based checks rather than a full CTest suite. The repository includes detector-specific
C++ test files and the application can be run against them from the command line to inspect warnings and output.

## Current Limitations

- Some detectors are pattern-based rather than full semantic analysis.
- Call-graph and points-to analysis is conservative and partial.
- Data-clump naming depends on network access and a valid Gemini API key.
- Some warnings are informational and do not include automated edits.
- The project is focused on the implemented smell checks rather than every possible C++ code pattern.

## Team

- Stuart Arief
- Rebecca Jennings
- Jimmy Munoz
- Aelynn Xu

The project website in [index.html](index.html) contains the most detailed architecture notes, diagrams, and project progress log.
