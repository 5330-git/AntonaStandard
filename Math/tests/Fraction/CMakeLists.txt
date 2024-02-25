cmake_minimum_required(VERSION 3.10)

include(CTest)
include(GoogleTest)

# 生成测试程序
add_executable(Test_Fraction Test_Fraction.cpp )

# 连接GTest 和 GTestMain 以及 AntonaStandard
target_link_libraries(
    Test_Fraction 
    AntonaStandard::CPS.static
    AntonaStandard::Math.static
    GTest::gtest 
    GTest::gtest_main
)
gtest_add_tests(TARGET Test_Fraction)


