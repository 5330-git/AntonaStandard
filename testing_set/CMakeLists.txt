cmake_minimum_required(VERSION 3.10)
project(Testing_Set)

# 添加google test 库文件和头文件路径
set(GOOGLE_TEST_ROOT ${CMAKE_CURRENT_SOURCE_DIR}/../third_party_kit/GoogleTest)
set(ASD_ROOT ${CMAKE_CURRENT_SOURCE_DIR}/..)
file(GLOB ASD_SRC_FILES ${ASD_ROOT}/src/*.cpp)
message(${CMAKE_CURRENT_SOURCE_DIR})
# 设置debug版本
set(CMAKE_BUILD_TYPE Debug)
set(CMAKE_CXX_FLAG="-g")

# 开启代码覆盖率信息生成
SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fprofile-arcs -ftest-coverage -fno-access-control ")
SET(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fprofile-arcs -ftest-coverage")

# 设置生成测试版头文件
add_definitions(-DASD_TEST_VERSION)

include_directories(${GOOGLE_TEST_ROOT}/include)
include_directories(${ASD_ROOT}/include)

# 生成测试版库文件

# 根据不同的平台连接不同的库文件,并设置测试程序的输出路径
if(CMAKE_SYSTEM_NAME MATCHES "Windows")
    link_directories(${GOOGLE_TEST_ROOT}/lib/Windows)
    set(EXECUTABLE_OUTPUT_PATH ${CMAKE_CURRENT_SOURCE_DIR}/bin/Windows)
    add_definitions(-DTEST_PROJECT_DIR="${CMAKE_CURRENT_SOURCE_DIR}/bin/Windows/")

elseif(CMAKE_SYSTEM_NAME MATCHES "Linux")
    link_directories(${GOOGLE_TEST_ROOT}/lib/Linux)
    set(EXECUTABLE_OUTPUT_PATH ${CMAKE_CURRENT_SOURCE_DIR}/bin/Linux)
    add_definitions(-DTEST_PROJECT_DIR="${CMAKE_CURRENT_SOURCE_DIR}/bin/Linux/")

endif()

# 添加子文件夹
add_subdirectory(Math)
add_subdirectory(Utilities)
add_subdirectory(ThreadTools)
add_subdirectory(MultiPlatformSupport)