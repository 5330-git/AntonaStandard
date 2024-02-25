cmake_minimum_required(VERSION 3.15)

add_executable(Utilities_Reflection Utilities_Reflection.cpp)

set_target_properties(
    Utilities_Reflection
    PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/../../bin
)
target_link_libraries(
    Utilities_Reflection
    PUBLIC
    AntonaStandard::Utilities.static
    AntonaStandard::Globals.static
    AntonaStandard::CPS.static
)
