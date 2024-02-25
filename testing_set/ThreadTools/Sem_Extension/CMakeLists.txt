cmake_minimum_required(VERSION 3.10)
project(Test_Sem_Extension)

# 设置CXX版本为 20（需要使用C++20 的信号量）
set(CMAKE_CXX_STANDARD 20)

# 生成测试程序
add_executable(Test_Sem_Extension Test_Sem_Extension.cpp)
# 链接库
target_link_libraries(Test_Sem_Extension gtest gtest_main)
