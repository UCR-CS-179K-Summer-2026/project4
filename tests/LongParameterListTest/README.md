# LongParameterListTest

Detector-aligned positive and negative cases for the current `Long Parameter List` check.

- `LongParameterListTest.cpp` is the positive case: `calculateOrderTotal` has 5 parameters and must produce one warning.
- `NotLongParameterListTest1.cpp` through `NotLongParameterListTest4.cpp` are clean controls with 2, 4, 2, and 2 parameters respectively; each must produce zero warnings.
- Run the five `.cpp` files through `build/Debug/project4.exe`; headers are declarations only.
