cmake_minimum_required(VERSION 3.15)

add_executable(Utilities_Delegate Utilities_Delegate.cpp)

set_target_properties(
    Utilities_Delegate 
    PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/../../bin
)
target_link_libraries(
    Utilities_Delegate 
    PUBLIC
    AntonaStandard::Utilities.static
    AntonaStandard::CPS.static
)
