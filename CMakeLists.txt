cmake_minimum_required(VERSION 3.10)

project(project4)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

add_subdirectory(tree-sitter)

# Disable curl features/dependencies that we don't need
set(BUILD_CURL_EXE OFF CACHE BOOL "" FORCE)
set(BUILD_TESTING OFF CACHE BOOL "" FORCE)

set(HTTP_ONLY ON CACHE BOOL "" FORCE)

set(CURL_USE_LIBPSL OFF CACHE BOOL "" FORCE)
set(CURL_USE_LIBSSH2 OFF CACHE BOOL "" FORCE)
set(CURL_USE_LIBIDN2 OFF CACHE BOOL "" FORCE)
set(USE_NGHTTP2 OFF CACHE BOOL "" FORCE)

set(CURL_ZLIB OFF CACHE BOOL "" FORCE)
set(CURL_BROTLI OFF CACHE BOOL "" FORCE)
set(CURL_ZSTD OFF CACHE BOOL "" FORCE)

if(WIN32)
    set(CURL_USE_SCHANNEL ON CACHE BOOL "" FORCE)
endif()

add_subdirectory(curl)

add_library(tree-sitter-cpp
    tree-sitter-cpp/src/parser.c
    tree-sitter-cpp/src/scanner.c
)

include(FetchContent)

FetchContent_Declare(
    json
    GIT_REPOSITORY https://github.com/nlohmann/json.git
    GIT_TAG v3.12.0
)

FetchContent_MakeAvailable(json)

set(DETECTOR_SOURCES
    parser.cpp
    poorNameChecker.cpp
    SmellyCodeDetector.cpp
    redundantCodeChecker/redundantCodeChecker.cpp
    deadCodeChecker/deadCodeChecker.cpp
    deadCodeChecker/pointsToAnalyzer.cpp
    repeatedCodeChecker.cpp
    commentChecker/commentChecker.cpp
    FunctionAnalyzer.cpp
    NameAnalyzer.cpp
    DeepIfDetector.cpp
    DataClumpDetector.cpp
    memoryChecker/memoryChecker.cpp
    longParamList.cpp
    inheritanceChecker.cpp
    DataClumpNameGen.cpp
    FixApplier.cpp
)

add_library(detector_core ${DETECTOR_SOURCES})
target_include_directories(detector_core PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
)

add_executable(project4 main.cpp)

target_include_directories(tree-sitter-cpp PUBLIC
    tree-sitter-cpp/src
)

target_link_libraries(detector_core PUBLIC
    tree-sitter
    tree-sitter-cpp
    libcurl
    nlohmann_json::nlohmann_json
)

target_link_libraries(project4 PRIVATE detector_core)

enable_testing()
add_subdirectory(tests)