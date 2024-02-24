cmake_minimum_required(VERSION 3.10)
project(ThreadConcurrentTools)

# 包含头文件路径
include_directories(${IncludePATH})
# 本项目特殊，需要使用c++20标准
set(CMAKE_CXX_STANDARD 20)
# 生成动态链接的可执行文件
add_executable(demo_ThreadsPool demo.cpp)
target_link_libraries(demo_ThreadsPool SAntonaStandard)