cmake_minimum_required(VERSION 3.10)
project(Reflection)

# 包含头文件路径
include_directories(${IncludePATH})
# 生成动态链接的可执行文件
add_executable(demo_${PROJECT_NAME} demo.cpp)
target_link_libraries(demo_${PROJECT_NAME} DAntonaStandard)