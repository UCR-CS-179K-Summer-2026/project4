# DataClassTest

Detector-aligned data-oriented comparison. The original Data Class smell is not implemented by the project, so the positive files exercise supported checks found in data-processing code.

- `DataClassTest1.cpp` is positive for `Long Parameter List` and `Redundant boolean comparison` and must produce two warnings.
- `DataClassTest2.cpp` is positive for `deep-if` and `repeated-code` and must produce two warnings.
- `NotDataClassTest1.cpp`, `NotDataClassTest2.cpp`, and `NotDataClassTest3.cpp` are clean controls and must produce zero warnings.
- Run the five `.cpp` files through `build/Debug/project4.exe`; headers are retained only as folder context.
