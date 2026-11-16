cmake_minimum_required(VERSION 3.20)

project(Exp_08_SystemC_SC_THREAD LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

find_package(SystemCLanguage CONFIG REQUIRED)

add_executable(Experiment_08 main.cpp)

target_link_libraries(Experiment_08
    PRIVATE SystemC::systemc
)
