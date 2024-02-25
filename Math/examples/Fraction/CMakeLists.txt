cmake_minimum_required(VERSION 3.15)

add_executable(Math_Fraction Math_Fraction.cpp)

set_target_properties(
    Math_Fraction
    PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/../../bin
)
target_link_libraries(
    Math_Fraction
    PUBLIC
    AntonaStandard::Math.static
    AntonaStandard::CPS.static
)
