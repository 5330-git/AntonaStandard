cmake_minimum_required(VERSION 3.15)

set(component_name Network)

build_component(${component_name})

# 调用顶层CMakeLists.txt 定义的函数，配置导出以及安装

set_and_install_library_files(
    ${component_name}
)

set_and_install_library_files(
    ${component_name}.static
)

target_link_libraries(
    ${component_name}.static 
    PUBLIC
    AntonaStandard::CPS.static
)

target_link_libraries(
    ${component_name}
    PUBLIC
    AntonaStandard::CPS
)

# 根据用户传入的参数决定是否构建示例
if(BUILD_EXAMPLES)
    add_subdirectory(examples)
endif()

if(BUILD_TESTS)
    add_subdirectory(tests)
endif()
