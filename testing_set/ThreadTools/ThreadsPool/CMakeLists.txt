cmake_minimum_required(VERSION 3.10)
project(Test_ThreadsPool)

# 生成测试程序
add_executable(Test_ThreadsPool Test_ThreadsPool.cpp ${ASD_ROOT}/src/ThreadsPool.cpp )

# 链接库
target_link_libraries(Test_ThreadsPool gtest gtest_main)