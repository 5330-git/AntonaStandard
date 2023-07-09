cmake_minimum_required(VERSION 3.10)
project(Antona_foreach)

# 包含头文件路径
include_directories(${IncludePATH})
# 生成动态链接的可执行文件
add_executable(demo_${PROJECT_NAME} demo.cpp)
# 该可执行文件不包含源文件，可以不链接库文件
target_link_libraries(demo_${PROJECT_NAME} DAntonaStandard)