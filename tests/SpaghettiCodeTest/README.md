# SpaghettiCodeTest

Detector-aligned comparison retaining the original spaghetti-code theme.

- `SpaghettiCodeTest1.cpp` and `SpaghettiCodeTest2.cpp` are positive cases. Their inline repeated statement runs must produce repeated-code warnings.
- `NotSpaghettiCodeTest.cpp` is the clean control and must produce zero warnings.
- The project has no dedicated spaghetti-code detector, so the supported proxy criterion is repeated code within a function.
- Run the three `.cpp` files through `build/Debug/project4.exe`; answer `n` to any fix prompt.
