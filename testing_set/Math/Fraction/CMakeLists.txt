cmake_minimum_required(VERSION 3.10)
project(Test_Fraction)


# 生成测试程序
add_executable(Test_Fraction Test_Fraction.cpp ${ASD_ROOT}/src/Fraction.cpp )

# 连接GTest 和 GTestMain 以及 AntonaStandard
target_link_libraries(Test_Fraction gtest gtest_main)

