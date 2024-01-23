cmake_minimum_required(VERSION 3.10)
project(AntonaStandardExamples)


# 只有指定编译示例程序时才编译
if (ASD_BUILD_EXAMPLE)
    # 链接库文件的目录
    if(CMAKE_HOST_SYSTEM_NAME MATCHES "Linux")
        set(EXECUTABLE_OUTPUT_PATH ${BinPATH}/Linux)
        link_directories(${LibPATH}/Linux)
    elseif(CMAKE_HOST_SYSTEM_NAME MATCHES "Windows")
        set(EXECUTABLE_OUTPUT_PATH ${BinPATH}/Windows)
        link_directories(${LibPATH}/Windows)
    endif()

    # 添加子文件夹
    add_subdirectory(Antona_foreach)
    add_subdirectory(Math)
    add_subdirectory(Delegate)
    add_subdirectory(Sem_Extension)
    add_subdirectory(ThreadsPool)
    add_subdirectory(DllSupport)
    # add_subdirectory(SocketSupport)
    add_subdirectory(Network)
endif()