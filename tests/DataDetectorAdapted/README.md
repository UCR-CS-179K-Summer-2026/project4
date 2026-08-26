# DataDetectorAdapted

Detector-aligned data-oriented suite adapted from the existing real-world `DataClassTest` suite. The original Data Class smell is not implemented, so the focused positives exercise supported checks found in data-processing code.

- `data_detector_positive_1.cpp` is positive for `Long Parameter List` and `Redundant boolean comparison` and must produce two warnings.
- `data_detector_positive_2.cpp` is positive for `deep-if` and `repeated-code` and must produce two warnings.
- `clean_control_1.cpp`, `clean_control_2.cpp`, and `clean_control_3.cpp` are clean controls and must produce zero warnings.
- `detector_variety.cpp` exercises nine supported categories: Data Clump, Long Parameter List, redundant boolean comparison, repeated code, deep-if, memory leak, unused functions, and unused inheritance.
- Run the six `.cpp` files through `build/Debug/project4.exe`; headers are retained only as folder context.
