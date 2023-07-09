cmake_minimum_required(VERSION 3.10)
project(AntonaStandard)
# 以下生成库文件
# 搜索源文件
file(GLOB SRC ${SrcPATH}/*.cpp)
# 包含头文件路径
include_directories(${IncludePATH})



# 分别在Windows平台和Linux平台下生成库文件
if(CMAKE_HOST_SYSTEM_NAME MATCHES "Linux")
    # 库文件重定向
    set(LIBRARY_OUTPUT_PATH ${LibPATH}/Linux)
elseif(CMAKE_HOST_SYSTEM_NAME MATCHES "Windows")
    set(LIBRARY_OUTPUT_PATH ${LibPATH}/Windows)
endif()

# 生成动态库
add_library(D${PROJECT_NAME} SHARED ${SRC})     # libDAntonaStandard.so 或 libDAntonaStandard.dll libDAntonaStandard.dll.a
# 生成静态库
add_library(S${PROJECT_NAME} STATIC ${SRC})     # libSAntonaStandard.a
