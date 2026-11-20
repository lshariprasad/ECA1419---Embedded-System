cmake_minimum_required(VERSION 3.20)

project(Exp09_SystemC_TLM_ProductionLine LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

find_package(SystemCLanguage CONFIG REQUIRED)

add_executable(exp09_tlm main.cpp)
target_link_libraries(exp09_tlm PRIVATE SystemC::systemc)

if(MSVC)
    target_compile_options(exp09_tlm PRIVATE /W4)
else()
    target_compile_options(exp09_tlm PRIVATE -Wall -Wextra -Wpedantic)
endif()
