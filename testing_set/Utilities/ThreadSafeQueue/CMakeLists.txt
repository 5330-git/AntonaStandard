cmake_minimum_required(VERSION 3.10)
project(Test_ThreadSafeQueue)

include_directories(${ASD_ROOT}/include)

# 生成测试程序
add_executable(Test_ThreadSafeQueue Test_ThreadSafeQueue.cpp )

# 链接库
target_link_libraries(Test_ThreadSafeQueue gtest gtest_main )