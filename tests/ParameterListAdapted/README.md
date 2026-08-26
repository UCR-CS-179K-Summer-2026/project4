# ParameterListAdapted

Detector-aligned suite adapted from the existing real-world `LongParameterListTest` suite. It keeps the focused parameter-count comparison and adds a multi-detector variety case.

- `long_parameter_positive.cpp` is the focused positive case: `calculateOrderTotal` has 5 parameters and must produce one warning.
- `clean_control_1.cpp` through `clean_control_4.cpp` are clean controls with short parameter lists; each must produce zero warnings.
- `detector_variety.cpp` exercises nine supported categories: Data Clump, Long Parameter List, redundant boolean comparison, repeated code, deep-if, memory leak, unused functions, and unused inheritance.
- Run the six `.cpp` files through `build/Debug/project4.exe`; headers are declarations only.
