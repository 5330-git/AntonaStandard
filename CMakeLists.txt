cmake_minimum_required(VERSION 3.15)

set(ROOT_PROJECT_NAME AntonaStandard)
set(ROOT_PROJECT_VERSION 11.0.0)


project(${ROOT_PROJECT_NAME} VERSION ${ROOT_PROJECT_VERSION})

# 打印项目信息
message(STATUS "Project: ${ROOT_PROJECT_NAME}")
message(STATUS "Version: ${ROOT_PROJECT_VERSION}")
message(STATUS "--> Major Version: ${${ROOT_PROJECT_NAME}_VERSION_MAJOR}")
message(STATUS "--> Minor Version: ${${ROOT_PROJECT_NAME}_VERSION_MINOR}")
message(STATUS "--> Patch Version: ${${ROOT_PROJECT_NAME}_VERSION_PATCH}")

# 设置选项，让用户决定是否编译测试和示例
option(BUILD_TESTS "Set this to enable building tests" OFF)
option(BUILD_EXAMPLES "Set this to enable building examples" OFF)

# 如果允许测试必须添加以下设置
if(${BUILD_TESTS})
  message(STATUS "Building tests")
  enable_testing()
endif()

# 设置各类文件的输出路径，默认输出到BUILD 目录下
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${PROJECT_BINARY_DIR}")       # 可执行文件以及windows下的动态库       
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${PROJECT_BINARY_DIR}")       # Linux 下的动态库 
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${PROJECT_BINARY_DIR}")       # 静态库以及Windows下的导出库 *.dll.a 

# 配置项目信息的头文件
configure_file(
  ${CMAKE_CURRENT_SOURCE_DIR}/AntonaStandardInfo.h.in
  AntonaStandardInfo.h
)

# 将配置信息的头文件安装到用户指定的安装目录下
install(
  FILES ${PROJECT_BINARY_DIR}/AntonaStandardInfo.h
  DESTINATION include/${ROOT_PROJECT_NAME}-${ROOT_PROJECT_VERSION}
)

# 定义头文件设置和安装的函数
function(set_and_install_header_files module_name)
  message(STATUS "The following headers will be installed at ${CMAKE_INSTALL_PREFIX}/include/${ROOT_PROJECT_NAME}-${ROOT_PROJECT_VERSION}/${module_name}")
  message(STATUS "--> ${ARGN}")
  install(
    FILES ${ARGN}
    DESTINATION include/${ROOT_PROJECT_NAME}-${ROOT_PROJECT_VERSION}/${module_name}
  )
endfunction(set_and_install_header_files)


# 定义库文件设置和安装的函数
function(set_and_install_library_files lib_name )
  message(STATUS "The following Targets will be installed at ${CMAKE_INSTALL_PREFIX}/lib")
  message(STATUS "--> ${lib_name}")
  set(prefix_name ${ROOT_PROJECT_NAME}-${lib_name})
  set(full_name ${prefix_name}-${ROOT_PROJECT_VERSION})

  # 为库文件改名，改成全称
  set_target_properties(
    ${lib_name}
    PROPERTIES
    OUTPUT_NAME
    ${full_name}
  )

  # 检查函数是否有额外的参数，如果有则说明需要设置为接口目标
  set(mode)
  if("${ARGN}" STREQUAL "")
      set(mode PUBLIC)
  else()
      set(mode INTERFACE)
  endif()
  message(STATUS "************************************************************************")
  message(STATUS "Target Include Directories mode is ${mode}")
  # 为库文件链接头文件（需要根据此时处于构建目录还是安装目录分别设置，（使用生成器表达式））
  target_include_directories(
    ${lib_name}
    ${mode}
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include/${ROOT_PROJECT_NAME}-${ROOT_PROJECT_VERSION}>
  )

  # 安装库文件,包括额外传入的需要导出的系统库
  install(
    TARGETS ${lib_name} 
    EXPORT ${lib_name}Targets
    DESTINATION lib
  )

  # 导出对应的cmake文件
  install(
    EXPORT ${lib_name}Targets
    FILE ${lib_name}Targets.cmake
    NAMESPACE ${ROOT_PROJECT_NAME}::
    DESTINATION lib/cmake/${full_name} 
  )

  message(STATUS "************************************************************************")
  message(STATUS "Export ${lib_name}Targets as \"${CMAKE_CURRENT_BINARY_DIR}/${lib_name}Targets.cmake\"")

  # 设置配置文件，让项目支持使用find_package 的Config 模式
  include(CMakePackageConfigHelpers)

  configure_package_config_file(
    ${CMAKE_CURRENT_SOURCE_DIR}/${lib_name}Config.cmake.in
    ${CMAKE_CURRENT_BINARY_DIR}/${prefix_name}Config.cmake
    INSTALL_DESTINATION lib/cmake/${full_name}
    NO_SET_AND_CHECK_MACRO
    NO_CHECK_REQUIRED_COMPONENTS_MACRO
  )

  # 设置版本信息文件
  write_basic_package_version_file(
    ${CMAKE_CURRENT_BINARY_DIR}/${prefix_name}ConfigVersion.cmake
    VERSION ${ROOT_PROJECT_VERSION}
    COMPATIBILITY SameMajorVersion
  )

  # 安装所有配置文件
  install(
    FILES
    ${CMAKE_CURRENT_BINARY_DIR}/${prefix_name}Config.cmake
    ${CMAKE_CURRENT_BINARY_DIR}/${prefix_name}ConfigVersion.cmake
    DESTINATION lib/cmake/${full_name} 
    COMPONENT ${lib_name}
  )
endfunction(set_and_install_library_files)


function(build_component component_name)
    # 分别查找头文件和源文件
    file(
        GLOB HEADERS_FILES 
        ${CMAKE_CURRENT_SOURCE_DIR}/include/${component_name}/*.h
        ${CMAKE_CURRENT_SOURCE_DIR}/include/${component_name}/*.hpp
    )

    file(GLOB SRC_FILES ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp)

    # 判断是否有源文件，如果没有源文件则添加为接口库
    if("${SRC_FILES}" STREQUAL "")
        message(STATUS "Building Component ${component_name} as an Interface")    
        add_library(${component_name}.static INTERFACE)
        add_library(${component_name} INTERFACE)
    else()
        # 生成静态库
        add_library(${component_name}.static STATIC ${SRC_FILES})
        # 生成动态库
        add_library(${component_name} SHARED ${SRC_FILES})
    endif()
    
    # 起别名,这样其它组件就可以直接通过 ${ROOT_PROJECT_NAME}::${component_name} 进行链接
    add_library(${ROOT_PROJECT_NAME}::${component_name} ALIAS ${component_name})
    add_library(${ROOT_PROJECT_NAME}::${component_name}.static ALIAS ${component_name}.static)
    set_and_install_header_files(
        ${component_name}
        ${HEADERS_FILES}
    )
endfunction(build_component)

# 安装顶层的配置文件
include(CMakePackageConfigHelpers)

configure_package_config_file(
  ${CMAKE_CURRENT_SOURCE_DIR}/${ROOT_PROJECT_NAME}Config.cmake.in
  ${CMAKE_CURRENT_BINARY_DIR}/${ROOT_PROJECT_NAME}Config.cmake
  INSTALL_DESTINATION lib/cmake/${ROOT_PROJECT_NAME}
)

# 设置版本信息配置文件
write_basic_package_version_file(
  ${CMAKE_CURRENT_BINARY_DIR}/${ROOT_PROJECT_NAME}ConfigVersion.cmake
  VERSION ${ROOT_PROJECT_VERSION}
  COMPATIBILITY SameMajorVersion
)

# 安装顶层的配置文件
install(
  FILES 
  ${CMAKE_CURRENT_BINARY_DIR}/${ROOT_PROJECT_NAME}Config.cmake
  ${CMAKE_CURRENT_BINARY_DIR}/${ROOT_PROJECT_NAME}ConfigVersion.cmake
  DESTINATION lib/cmake/${ROOT_PROJECT_NAME}
)

# 添加子文件夹
add_subdirectory(Globals)
add_subdirectory(TestingSupport)
add_subdirectory(CPS)
add_subdirectory(Network)
add_subdirectory(Utilities)
add_subdirectory(Math)
add_subdirectory(ThreadTools)

# 配置测试（编译GoogleTest 并设置安装目录为 external）
if(BUILD_TESTS)
    enable_testing()
    add_subdirectory(third_party/GoogleTest)
    set(GTEST_BINARY_DIR ${CMAKE_BINARY_DIR}/external/GoogleTest-build)
    set(GTEST_INSTALL_DIR ${CMAKE_BINARY_DIR}/external/GoogleTest-install)
endif(BUILD_TESTS)
# 配置卸载

# uninstall target
if(NOT TARGET uninstall)
  configure_file(
    "${CMAKE_CURRENT_SOURCE_DIR}/cmake_uninstall.cmake.in"
    "${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake"
    IMMEDIATE @ONLY)

  add_custom_target(uninstall
    COMMAND ${CMAKE_COMMAND} -P ${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake)
endif()
