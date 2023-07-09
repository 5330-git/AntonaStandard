cmake_minimum_required(VERSION 3.10)
# 该文件将使用cmake构建动态库和静态库
project(AntonaStandard)
# 设置生成路径的变量
set(ProjectPATH ${CMAKE_CURRENT_SOURCE_DIR})    # 所有的文件都将基于PROPATH进行生成
set(LibPATH ${ProjectPATH}/lib)                 # 库文件路径
set(IncludePATH ${ProjectPATH}/include)         # 头文件路径
set(SrcPATH ${ProjectPATH}/src)                 # 源文件路径
set(BinPATH ${ProjectPATH}/bin)

# 设置子文件夹

add_subdirectory(examples)
add_subdirectory(lib)


