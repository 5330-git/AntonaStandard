cmake_minimum_required(VERSION 3.15)

set(component_name Math)

build_component(${component_name})

set_and_install_library_files(
    ${component_name}
)

set_and_install_library_files(
    ${component_name}.static
)

# 该组件依赖 TestingSupport
target_link_libraries(
    ${component_name}.static
    PUBLIC
    AntonaStandard::TestingSupport.static
    AntonaStandard::Globals.static
)
target_link_libraries(
    ${component_name}
    PUBLIC
    AntonaStandard::TestingSupport
    AntonaStandard::Globals
)

# 根据用户传入的参数决定是否构建示例
if(BUILD_EXAMPLES)
    add_subdirectory(examples)
endif()

if(BUILD_TESTS)
    add_subdirectory(tests)
endif()
