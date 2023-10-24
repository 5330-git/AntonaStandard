# AntonaStandard

#### 介绍
自定义的库，里面编写了一些常用的工具

#### 安装教程

1. 需要手动构建库文件,以下命令将同时对示例代码进行构建（为了保证路径的整洁，建议在build目录下构建）：

   **Linux(Ubuntu 22.04)**: 

   - 进入`build` 目录，输入以下命令

     ```bash
     rm * -rf # 首先清空build目录
     cmake ../ # 构建生成makefile文件
     make # 执行make命令，开始构建库文件和示例文件
     ```

   - 库文件将生成到 `lib` 目录下

   **Windows(10)**:

   - 首先打开 `PowerShell` 

   - 进入项目的`build`目录，输入以下命令

     ```powershell
     Remove-Item -Path ./* # 清空build目录
     cmake .. -G "MinGW Makefiles" # 指定makefile的构建工具，Windows下CMake默认生成VS的构建文件
     mingw32-make.exe # 更据build下的Makefile文件自动构建库文件和示例文件
     ```

2. 库文件使用方法

   **G++** 命令

   - 下面演示用g++链接静态库文件

   ```bash
   # g++ 源文件名 静态库路径以及文件名 -I 头文件路径
   g++ src.cpp /AntonaStandard/lib/Linux/libSAntonaStnadard.a -I /AntonaStandard/include -o app
   # 注意根据平台的不同选择不同的库文件
   ```

   - 下面演示用g++链接动态库文件

   ```bash
   g++ src.cpp -L /AntonaStandard/lib/Linux/ -fDAntonaStandard -Wl.rpath=/AntonaStandard/lib/Linux/ -o app
   # 注意根据平台的不同选择不同的库文件
   ```

   **CMake** 编写示例

   ```cmake
   cmake_minimum_required(VERSION 3.10)
   project(Test)
   
   set(AntonaStandardPath 填写自己下载AntonaStandard的路径)
   # 包含头文件路径
   include_directories(${AntonaStandardPath}/include)
   # 对于部分库可能需要使用c++20标准
   ## set(CMAKE_CXX_STANDARD 20)
   # 链接库文件目录
   if(CMAKE_HOST_SYSTEM_NAME MATCHES "Linux")
       link_directories(${AntonaStandardPath}/lib/Linux)
   elseif(CMAKE_HOST_SYSTEM_NAME MATCHES "Windows")
       link_directories(${AntonaStandardPath}/lib/Windows)
   endif()
   
   	
   # 生成动态链接的可执行文件
   add_executable(dynamic_demo 填写测试源文件名称)
   target_link_libraries(dynamic_demo DAntonaStandard)
   # 生成静态链接的可执行文件
   add_executable(static_demo 填写测试源文件名称)
   target_link_libraries(static_demo SAntonaStandard)
   ```


**另外，Windows下的示例程序可能会提示找不到动态库，可以选择将 `./bin/Windows` 下的库文件拷贝到 `bin` 目录下**





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

2023/4/12   AntonaStandard-v-7.0.0 添加线程并发工具库——信号量扩展，给予C++20 semaphore库，实现And信号量请求，与信号量集请求

2023/7/9   AntonaStandard-v-7.1.0 完善了cmake构建文件，可以根据平台的不同构建不同版本的库文件和示例源文件。同时对反射库Rflection 类型过滤器Filter 信号量扩展Sem_Extension进行了一定的修改

2023/8/8 AntonaStandard-v-8.0.0 修改大量文件的命名空间，修改了include下的文件结构，新增线程池库

2023/8/11 AntonaStandard-v-8.0.1 添加 `.gitignore` 文件，忽略`bin`目录和`build`目录以及 `lib` （这意味着自版本 v-8.0.1起不再提供提前编译好的库文件）

2023/8/17 AntonaStandard-v-9.0.0 `.gitignore`新增需要git忽略的目录，新增跨平台支持（目前添加支持跨平台的动态库显式加载（热加载）的API），更新异常库，同时优化了文件目录

2023/9/15 AntonaStandard-v-9.1.0-Beta `.gitignore`新增需要git忽略的目录，新增跨平台支持：跨平台的TCP/IPV4 套接字通信，当前版本为测试版本，后续版本可能对封装的套接字库接口有加大的改动

2023/9/16 AntonaStandard-v-9.1.0 重新设计了封装了ip地址和端口的类，实现简单易用的跨平台的**TCP/IPV4** 以及 **TCP/IPV6** 套接字通信方案

2023/10/24 AntonaStandard-v-9.2.0 过渡版本

#### 原作者博客



- [学艺不精的Антон的博客_CSDN博客](https://blog.csdn.net/yyy11280335?spm=1000.2115.3001.5343) 
