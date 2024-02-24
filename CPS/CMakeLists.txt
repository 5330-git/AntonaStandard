cmake_minimum_required(VERSION 3.15)
set(component_name CPS)

set(SYSTEM_LIBS )
if(WIN32)
    # 添加ws2_32库
    set(SYSTEM_LIBS ws2_32)
endif()

build_component(${component_name})

# 调用顶层CMakeLists.txt 定义的函数，配置导出以及安装

set_and_install_library_files(
    ${component_name}
)

set_and_install_library_files(
    ${component_name}.static
)

# 链接依赖：AntonaStandard::Globals
target_link_libraries(
    ${component_name}.static 
    PUBLIC
    AntonaStandard::Globals.static
    ${SYSTEM_LIBS}
)

target_link_libraries(
    ${component_name}
    PUBLIC
    AntonaStandard::Globals
    ${SYSTEM_LIBS}
)
