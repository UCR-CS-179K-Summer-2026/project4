# Universal annotated scenario

This folder is a multi-file universal test for the detectors currently registered in `SmellyCodeDetector`.

- `universal_smelly.cpp` intentionally exercises poor naming, redundant boolean comparison, repeated code, missing comments, deep conditionals, data clumps, memory leaks, long parameter lists, unused functions, and unused inheritance.
- `universal_clean.cpp` is a clean control expected to produce no warnings.
- The scenario is run through `build/Debug/project4.exe`, which accepts space-separated source paths on its first input line. Answer `n` to each automatic-fix prompt so the fixtures remain unchanged.
- Some detectors depend on whole-program context or exact AST patterns, so the manifest is an expectation of coverage rather than a claim that every annotated smell must be reported.

## Expected coverage

`universal_smelly.cpp` contains an intentional case for every currently registered detector: `Data Clump`, `Comments`, `Poor Naming`, `Redundant boolean comparison`, `repeated-code`, `deep-if`, `Long Parameter List`, `memory-leak`, `unused-function`, and `unused-inheritance`.

The clean control is expected to pass with zero warnings. The smelly scenario is expected to fail the clean-report check while producing at least one warning in each of those ten categories.

## Recorded run

Using the rebuilt `build/Debug/project4.exe`, the smelly scenario produced 20 warnings across all ten categories. The clean control produced 0 warnings. No fixes were applied. The missing API-key fallback in `DataClumpNameGen` returns the deterministic name `DataGroup`, allowing this test to run offline.
