cmake_minimum_required(VERSION 3.10)
project(project4)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

add_subdirectory(tree-sitter)
add_library(tree-sitter-cpp tree-sitter-cpp/src/parser.c 
    tree-sitter-cpp/src/scanner.c
)

set(SOURCE_FILES
    main.cpp
    parser.cpp
    poorNameChecker.cpp
    poorNameTest.cpp
    SmellyCodeDetector.cpp
    redundantCodeChecker.cpp
    deadCodeChecker.cpp
    repeatedCodeChecker.cpp
    commentChecker.cpp
    FunctionAnalyzer.cpp
    NameAnalyzer.cpp
    DeepIfDetector.cpp
    longParamList.cpp
)
add_executable(project4 ${SOURCE_FILES})

target_include_directories(tree-sitter-cpp PUBLIC tree-sitter-cpp/src)

target_link_libraries(project4 PRIVATE tree-sitter
    tree-sitter-cpp
)