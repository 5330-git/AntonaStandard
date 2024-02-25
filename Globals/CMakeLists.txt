cmake_minimum_required(VERSION 3.15)
set(component_name Globals)

# 构建库文件
build_component(${component_name})

# 调用顶层CMakeLists.txt 定义的函数，配置导出以及安装


set_and_install_library_files(
    ${component_name}.static
)

set_and_install_library_files(
    ${component_name}
)
