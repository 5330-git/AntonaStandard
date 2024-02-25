cmake_minimum_required(VERSION 3.15)

add_executable(CPS_Dll CPS_Dll.cpp)

set_target_properties(
    CPS_Dll 
    PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/../../bin
)
target_link_libraries(
    CPS_Dll 
    PUBLIC
    AntonaStandard::CPS.static
)

# 生成展示的导出动态库
add_library(funcs SHARED funcs.cpp)
set_target_properties(
    funcs
    PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/demolib
    LIBRARY_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/demolib
)
target_link_libraries(
    funcs
    PUBLIC
    AntonaStandard::CPS.static
)
