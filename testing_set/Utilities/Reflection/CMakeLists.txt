cmake_minimum_required(VERSION 3.10)
project(Test_Reflection)


# 生成测试程序
add_executable(Test_Reflection Test_Reflection.cpp ${ASD_ROOT}/src/Reflection.cpp )

# 链接库
target_link_libraries(Test_Reflection gtest )