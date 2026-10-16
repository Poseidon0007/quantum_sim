cmake_minimum_required(VERSION 3.20)
project(quantum_sim LANGUAGES CXX)

add_executable(quantum_sim src/main.cpp)
target_compile_features(quantum_sim PRIVATE cxx_std_20)