cmake_minimum_required(VERSION 3.0)
project(test_find_usage)
set(CMAKE_CXX_FLAGS "-Xclang -load -Xclang /home/alexwang/dev/exps/libclang-find-usage/cmake-build-debug/libfind-usage.so -Xclang -plugin -Xclang find-usages")
add_executable(test_find_usage test_find_usage.cpp)
