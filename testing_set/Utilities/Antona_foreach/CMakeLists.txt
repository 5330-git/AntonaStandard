cmake_minimum_required(VERSION 3.10)
project(Test_Antona_foreach)


# 生成测试文件
add_executable(Test_Antona_foreach Test_Antona_foreach.cpp )

# 链接库文件
target_link_libraries(Test_Antona_foreach gtest gtest_main)