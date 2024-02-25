cmake_minimum_required(VERSION 3.10)
project(Test_Delegate)


# 生成测试程序
add_executable(Test_Delegate Test_Delegate.cpp )
# 链接库
target_link_libraries(Test_Delegate gtest )