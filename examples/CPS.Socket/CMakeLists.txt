cmake_minimum_required(VERSION 3.10)
project(SocketSupport)

# 包含头文件路径
include_directories(${IncludePATH})
# 生成动态链接的可执行文件
add_executable(CPS_TcpC TcpClient.cpp)
add_executable(CPS_TcpS TcpServer.cpp)
add_executable(CPS_TcpCV6 TcpClientV6.cpp)
add_executable(CPS_TcpSV6 TcpServerV6.cpp)
add_executable(CPS_UdpC UdpClient.cpp)
add_executable(CPS_UdpS UdpServer.cpp)
add_executable(CPS_UdpCV6 UdpClientV6.cpp)
add_executable(CPS_UdpSV6 UdpServerV6.cpp)

target_link_libraries(CPS_TcpC SAntonaStandard)
target_link_libraries(CPS_TcpS SAntonaStandard)
target_link_libraries(CPS_TcpCV6 SAntonaStandard)
target_link_libraries(CPS_TcpSV6 SAntonaStandard)
target_link_libraries(CPS_UdpC SAntonaStandard)
target_link_libraries(CPS_UdpS SAntonaStandard)
target_link_libraries(CPS_UdpCV6 SAntonaStandard)
target_link_libraries(CPS_UdpSV6 SAntonaStandard)
