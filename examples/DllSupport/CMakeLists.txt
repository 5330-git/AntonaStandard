cmake_minimum_required(VERSION 3.10)
project(DllSupport)

# 包含头文件路径
include_directories(${IncludePATH})
# 生成动态链接的可执行文件
add_executable(demo_${PROJECT_NAME} demo.cpp)
target_link_libraries(demo_${PROJECT_NAME} SAntonaStandard)

#生成导出的动态库
set(PreformLibPath ${LIBRARY_OUTPUT_PATH})
set(LIBRARY_OUTPUT_PATH ${CMAKE_CURRENT_SOURCE_DIR}/demolib)
add_library(funcs SHARED funcs.cpp)