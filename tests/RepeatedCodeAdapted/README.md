# RepeatedCodeAdapted

Detector-aligned suite adapted from the existing real-world `SpaghettiCodeTest` suite. It retains the original tangled-code theme while naming the supported repeated-code criterion directly.

- `repeated_code_positive_1.cpp` and `repeated_code_positive_2.cpp` are positive cases. Their repeated statement runs must produce repeated-code warnings.
- `clean_control.cpp` is the negative control and must produce zero warnings.
- `detector_variety.cpp` exercises nine supported categories: Data Clump, Long Parameter List, redundant boolean comparison, repeated code, deep-if, memory leak, unused functions, and unused inheritance.
- Run the four `.cpp` files through `build/Debug/project4.exe`; answer `n` to any fix prompt.
