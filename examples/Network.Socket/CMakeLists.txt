cmake_minimum_required(VERSION 3.10)
project(Network)

# 包含头文件路径
include_directories(${IncludePATH})
# 生成动态链接的可执行文件
add_executable(demo_TcpServer TcpServer.cpp)
add_executable(demo_TcpClient TcpClient.cpp)

target_link_libraries(demo_TcpServer SAntonaStandard)
target_link_libraries(demo_TcpClient SAntonaStandard)