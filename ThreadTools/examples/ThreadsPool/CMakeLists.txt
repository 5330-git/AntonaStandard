cmake_minimum_required(VERSION 3.10)

file(GLOB SRCS ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp)
foreach(src ${SRCS})
    get_filename_component(src_name ${src} NAME_WLE) 
    set(src_name ThreadTools_${src_name})
    add_executable(${src_name} ${src})
    set_target_properties(
        ${src_name}
        PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/../../bin
        CXX_STANDARD 20
    )
    target_link_libraries(${src_name} 
        PUBLIC 
        AntonaStandard::CPS.static
        AntonaStandard::ThreadTools.static
    )
endforeach()

