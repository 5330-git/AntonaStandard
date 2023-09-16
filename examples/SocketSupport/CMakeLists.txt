cmake_minimum_required(VERSION 3.10)
project(SocketSupport)

# 包含头文件路径
include_directories(${IncludePATH})
# 生成动态链接的可执行文件
add_executable(demo_TcpClient TcpClient.cpp)
add_executable(demo_TcpServer TcpServer.cpp)
add_executable(demo_TcpClientV6 TcpClientV6.cpp)
add_executable(demo_TcpServerV6 TcpServerV6.cpp)

target_link_libraries(demo_TcpClient SAntonaStandard)
target_link_libraries(demo_TcpServer SAntonaStandard)
target_link_libraries(demo_TcpClientV6 SAntonaStandard)
target_link_libraries(demo_TcpServerV6 SAntonaStandard)
