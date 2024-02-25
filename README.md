# AntonaStandard

#### 介绍

**概述**

C++ 基础库，供学习C++底层原理以及CMake构建原理使用。目前支持数学分数库、基础反射机制、基础委托机制、跨平台动态库热加载、跨平台套接字库封装、支持ipv4 和ipv6 的TCP-UDP套接字高级封装。构建方面支持cmake find_package 的 Configure 模式。拥有单元测试，支持 CTest 和 Google Test 拥有比较完善的单元测试构建体系，但目前单元测试代码相对匮乏，欢迎参与本项目贡献基于Google Test 的单元测试代码！😉🙌

**子项目**

子项目被放置在根目录中的 `thrid_party` 目录下，其它有关子项目的具体信息可以查看项目根目录的 `.gitmodules ` 文件

- **Google Test** 
  - 本项目基于 `1.14.x` 版本的Google Test 实现单元测试，而 Google Test 是以子模块的形式引入本项目的，通过设置选项 `-DBUILD_TESTS=ON  ` 可以自动构建 Google Test 并与 `AntonaStandard` 安装到同一个目录下





#### 克隆仓库

- 需要使用 `--recurse-submodule` 选项同时克隆依赖的模块 (Google Test)

  ```bash
  git clone --branch master --recurse-submodule https://gitee.com/ordinaryAnton/antona-standard.git AntonaStandard
  ```

- 如果忘记了添加 `--recurse-submodule` 选项，也可以等待主项目克隆完毕后，进入项目目录使用以下命令克隆子项目

  ```bash
  git submodule update --recursive --remote
  ```

  

#### 安装教程

- 项目主要目录

  ```bash
  .
  ├── AntonaStandardConfig.cmake.in
  ├── AntonaStandardInfo.h.in
  ├── CMakeLists.txt
  ├── CPS
  ├── Globals
  ├── Improvement Report.md
  ├── LICENSE
  ├── Math
  ├── Network
  ├── README.en.md
  ├── README.md
  ├── TestingSupport
  ├── ThreadTools
  ├── Utilities
  ├── cmake_uninstall.cmake.in
  ├── docs
  ├── testing_set
  └── third_party
  ```

  



**Linux(Ubuntu 22.04)**: 

- 创建一个目录用于构建

  ```bash
  mkdir buildlin
  cd buildlin
  ```

- 运行 `cmake` 

  ```bash
  cmake .. -DBUILD_EXAMPLES=ON -DBUILD_TESTS=ON -DCMAKE_INSTALL_PREFIX=<path to install>
  ```

  - `-DBUILD_EXAMPLES=ON` ：允许构建示例程序，示例程序会生成在对应组件目录下的 `bin` 目录中
  - `-DBUILD_TESTS=ON` ：允许构建单元测试，同时还会编译Google Test
  - `-DCMAKE_INSTALL_PREFIX=<path to install` ：设置安装目录，如果不加该选项，默认安装到 `/usr/local`  目录下

- 运行 `make` 进行构建

  ```bash
  make -j 16
  ```

- 运行单元测试 

  ```bash
  make test
  ```

- 安装

  ```bash
  make install
  ```



- *卸载方法* 

  ```bash
  make uninstall
  ```

  

**Windows(10)**:

- 首先打开 `PowerShell` 

- 创建一个目录用于构建

  ```bash
  mkdir buildlin
  cd buildlin
  ```

- 运行 `cmake` 

  ```bash
  cmake .. -G 'MinGW Makefiles' -DBUILD_EXAMPLES=ON -DBUILD_TESTS=ON -DCMAKE_INSTALL_PREFIX=<path to install>
  ```
  
  - `-G 'MinGW Makefiles'` ：本项目还未对其它构建系统如 `MSVC` ，`LLVM` 进行测试，暂且只确定支持 `GNU` 以及 `MinGW`  
  - `-DBUILD_EXAMPLES=ON` ：允许构建示例程序，示例程序会生成在对应组件目录下的 `bin` 目录中
  
  - `-DBUILD_TESTS=ON` ：允许构建单元测试，同时还会编译Google Test
  
  - `-DCMAKE_INSTALL_PREFIX=<path to install` ：设置安装目录，如果不加该选项，默认安装到 `C:/Program Files (x86)/AntonaStandard`  目录下

- 运行 `make` 进行构建

  ```bash
  mingw32-make -j 16
  ```

- 运行单元测试 

  ```bash
  mingw32-make test
  ```

- 安装

  ```bash
  mingw32-make install
  ```



- *卸载方法* 

  ```bash
  mingw32-make uninstall
  ```

  

#### 简单使用

- ```cmake
  cmake_minimum_required(VERSION 3.15)
  
  # set(CMAKE_PREFIX_PATH ...../lib/cmake) 如果运行cmake时使用了-DCMAKE_INSTALL_PREFIX 选项则需要在这里设置packge查找路径
  
  find_package(
      AntonaStandard
      11.0.0
      REQUIRED
      Math
      Math.static
  )
  
  add_executable(demo_math_static demo_math_static.cpp)
  add_executable(demo_math demo_math.cpp)
  
  target_link_libraries(
      demo_math_static
      PUBLIC
      AntonaStandard::Math.static
  )
  
  target_link_libraries(
      demo_math
      PUBLIC
      AntonaStandard::Math
  )
  ```

- `demo_math.cpp` 和 `demo_math_static.cpp` 

  ```cpp
  #include <iostream>
  #include <Math/Fraction.h>
  using namespace std;
  using namespace AntonaStandard::Math;
  int main(){
      Fraction f1 = "1/2";
      Fraction f2 = "1/3";
      cout<<f1-f2<<endl;
      return 0;
  }
  
  ```





#### 历史版本
2022/12/30 AntonaStandard-v-1.0.0 添加了常用异常类库Exception.h和事件委托类库Delegate.h

2023/1/1     AntonaStandard-v-1.0.1 更新了Delegate.h详细请看具体文档

2023/1/1	 AntonaStandard-v-1.1.0 更新了Delegate.h详细请看具体文档

2023/1/2	 AntonaStandard-v-2.0.0 添加反射机制类库Reflection.h详细请看具体文档

2023/1/11   AntonaStandard-v-3.0.0 添加迭代器工具库Antona_foreach.h详细请看具体文档

2023/2/19   AntonaStandard-v-4.0.0 添加自动结点类库详细请看具体文档

2023/2/25   AntonaStandard-v-4.1.0 更新自动结点库，异常库

2023/2/27   AntonaStandard-v-5.0.0 添加AntonaStandard::AntonaMath库中的Fraction分数类库

2023/3/31   AntonaStandard-v-6.0.0 添加运行时类型识别库——过滤器Filter,实现RTTI以及处理

2023/4/12   AntonaStandard-v-7.0.0 添加线程并发工具库——信号量扩展，基于C++20 semaphore库，实现And信号量请求，与信号量集请求

2023/7/9   AntonaStandard-v-7.1.0 完善了cmake构建文件，可以根据平台的不同构建不同版本的库文件和示例源文件。同时对反射库Rflection 类型过滤器Filter 信号量扩展Sem_Extension进行了一定的修改

2023/8/8 AntonaStandard-v-8.0.0 修改大量文件的命名空间，修改了include下的文件结构，新增线程池库

2023/8/11 AntonaStandard-v-8.0.1 添加 `.gitignore` 文件，忽略`bin`目录和`build`目录以及 `lib` （这意味着自版本 v-8.0.1起不再提供提前编译好的库文件）

2023/8/17 AntonaStandard-v-9.0.0 `.gitignore`新增需要git忽略的目录，新增跨平台支持（目前添加支持跨平台的动态库显式加载（热加载）的API），更新异常库，同时优化了文件目录

2023/9/15 AntonaStandard-v-9.1.0-Beta `.gitignore`新增需要git忽略的目录，新增跨平台支持：跨平台的TCP/IPV4 套接字通信，当前版本为测试版本，后续版本可能对封装的套接字库接口有加大的改动

2023/9/16 AntonaStandard-v-9.1.0 重新设计了封装了ip地址和端口的类，实现简单易用的跨平台的**TCP/IPV4** 以及 **TCP/IPV6** 套接字通信方案

2023/10/24 AntonaStandard-v-9.2.0 过渡版本

2023/11/10 AntonaStandard-v-10.0.0 引入基于Google Test 的单元测试，保障代码的健壮性。使用智能指针替代裸指针，降低内存泄漏的风险。

2023/12/12 AntonaStandard-v-10.0.1 完成`MultiPlatformSupport::SocketSupport` 的UDP 基础功能

2024/2/25 AntonaStandard-v-11.0.0 完成对cmake module 的支持、完成对 CTest 和 Google Test 自动支持、完成ipv4 和ipv6 的TCP-UDP套接字高级封装（Network 组件）

#### 原作者博客



- [学艺不精的Антон的博客_CSDN博客](https://blog.csdn.net/yyy11280335?spm=1000.2115.3001.5343) 
