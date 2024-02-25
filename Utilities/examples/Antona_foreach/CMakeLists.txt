cmake_minimum_required(VERSION 3.15)

add_executable(Utilities_Antona_foreach Utilities_Antona_foreach.cpp)

set_target_properties(
    Utilities_Antona_foreach
    PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/../../bin
)
target_link_libraries(
    Utilities_Antona_foreach
    PUBLIC
    AntonaStandard::Utilities.static
    AntonaStandard::CPS.static
)
