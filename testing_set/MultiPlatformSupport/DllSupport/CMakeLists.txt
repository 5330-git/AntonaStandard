cmake_minimum_required(VERSION 3.10)
project(Test_DllSupport)

# 设置测试库文件路径
set(LIBRARY_OUTPUT_PATH ${EXECUTABLE_OUTPUT_PATH})

# 生成测试程序
add_executable(Test_DllSupport Test_DllSupport.cpp ${ASD_ROOT}/src/DllSupport.cpp)

# 链接库
target_link_libraries(Test_DllSupport gtest )

# 生成测试动态库
add_library(ImplementA SHARED ImplementA.cpp)
add_library(ImplementB SHARED ImplementB.cpp)