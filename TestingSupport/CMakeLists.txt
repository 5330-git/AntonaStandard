cmake_minimum_required(VERSION 3.15)

set(component_name TestingSupport)

build_component(${component_name})


set_and_install_library_files(
    ${component_name}
    INTERFACE
)

set_and_install_library_files(
    ${component_name}.static
    INTERFACE
)

