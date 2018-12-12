cmake_minimum_required(VERSION 3.11)
project(codetool)

set(CMAKE_CXX_STANDARD 17)
include_directories(${CMAKE_SOURCE_DIR}/include)
add_library(find_usages_pair_plugin SHARED
        src/plugins/find_usages_pair.cpp
        )
target_link_libraries(find_usages_pair_plugin clangAST clangBasic stdc++fs)

add_executable(find_usages_pair
        src/tools/find_usages_pair.cpp
        src/plugins/find_usages_pair.cpp
        )
target_link_libraries(
        find_usages_pair
        clangAST
        clangBasic
        clangCodeGen
        clangLex
        clangParse
        clangSema
        clangAnalysis
        clangDriver
        clangFrontend
        clangAST
        clangEdit
        clangSerialization
        clangTooling
        clangToolingCore
        clangAST
        clangParse
        clangBasic
        clangSema
        clangAST
        LLVM
        LLVMSupport
        stdc++fs)

