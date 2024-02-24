cmake_minimum_required(VERSION 3.10)
project(Test_SocketSupport)

# 生成测试程序
add_executable(Test_SocketSupport Test_SocketSupport.cpp ${ASD_ROOT}/src/SocketSupport.cpp)
# 链接库

target_link_libraries(Test_SocketSupport gtest gmock gtest_main)
if(CMAKE_SYSTEM_NAME MATCHES "Windows")
    target_link_libraries(Test_SocketSupport ws2_32)    
endif()