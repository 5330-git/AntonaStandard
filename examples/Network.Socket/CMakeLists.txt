cmake_minimum_required(VERSION 3.10)
project(Network)

# 包含头文件路径
include_directories(${IncludePATH})
# 生成动态链接的可执行文件
add_executable(Network_TcpS TcpServer.cpp)
add_executable(Network_TcpC TcpClient.cpp)
add_executable(Network_TcpSV6 TcpServerV6.cpp)
add_executable(Network_TcpCV6 TcpClientV6.cpp)
add_executable(Network_UdpS UdpServer.cpp)
add_executable(Network_UdpC UdpClient.cpp)
add_executable(Network_UdpSV6 UdpServerV6.cpp)
add_executable(Network_UdpCV6 UdpClientV6.cpp)

target_link_libraries(Network_TcpS SAntonaStandard)
target_link_libraries(Network_TcpC SAntonaStandard)
target_link_libraries(Network_TcpSV6 SAntonaStandard)
target_link_libraries(Network_TcpCV6 SAntonaStandard)
target_link_libraries(Network_UdpS SAntonaStandard)
target_link_libraries(Network_UdpC SAntonaStandard)
target_link_libraries(Network_UdpSV6 SAntonaStandard)
target_link_libraries(Network_UdpCV6 SAntonaStandard)