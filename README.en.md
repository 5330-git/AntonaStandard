# AntonaStandard

#### Introduction

​	This project is in order to create an code tool box(It is a supplement of **STL**). It includes some compnents that may help developer use some special characteristics which C++ doesn't have.Such as *Delegate* , *Reflection* and so on.



#### Installation 

1.  You can compile or build the project by yourself according the following command(To keep the directory clean,I suggest you compile or build this project under directory `build` , the `build` directory will be ignored if you add or commit the modification to the git store in default)

   **Linux(Ubuntu 22.04)**: 

   - change your current path to project directory `build1` .Then input these commands:

     ```bash
     rm * -rf 	# Clear the directory at first
     cmake ../ 	# Create Makefile script by cmake tool . avaliable item(compile the examples) -D ASD_BUILD_EXAMPLE=ON
     make 		# Build the source files of library and example 
     ```

   - Waiting for seconds , the library files will be write in directory `lib` 

   **Windows(10)**:

   - Firstly, open shell tool: `PowerShell`

   - change your current path to project directory `build` .Then input threse commands:

     ```powershell
     Remove-Item -Path ./* # Clear the direactory at first
     cmake .. -G "MinGW Makefiles" # Indicate the compile tools，because cmake create VS building files in defalut under platform Windows .avaliable item(compile the examples) -D ASD_BUILD_EXAMPLE=ON
     mingw32-make.exe # Build the project according to the Makefile script
     ```

   3. Usage of Library file

      **G++** Command

      - There is a demo of g++ command to link static library file

      ```bash
      # g++ {source files name} {static library file path and its name} -I {the path of headers}
      g++ src.cpp /AntonaStandard/lib/Linux/libSAntonaStnadard.a -I /AntonaStandard/include -o app
      # Attention: For different platform you should choose the different library path
      ```

      - There is a demo of g++ command to link dynamic library file

      ```bash
      g++ src.cpp -L /AntonaStandard/lib/Linux/ -fDAntonaStandard -Wl.rpath=/AntonaStandard/lib/Linux/ -o app
      # Attention: For different platform you should choose the different library path
      ```

      **CMake** script

      ```cmake
      cmake_minimum_required(VERSION 3.10)
      project(Test)
      
      set(AntonaStandardPath {your AntonaStandard path})
      # include headers direactory path
      include_directories(${AntonaStandardPath}/include)
      # for partial project, you should indicate the c++ standard by following command:
      ## set(CMAKE_CXX_STANDARD 20)
      # link the diractory of library
      if(CMAKE_HOST_SYSTEM_NAME MATCHES "Linux")
          link_directories(${AntonaStandardPath}/lib/Linux)
      elseif(CMAKE_HOST_SYSTEM_NAME MATCHES "Windows")
          link_directories(${AntonaStandardPath}/lib/Windows)
      endif()
      
      	
      # create excutable file by linking dynamic library
      add_executable(dynamic_demo {your test sources file names})
      target_link_libraries(dynamic_demo DAntonaStandard)
      # create excutable file by linking static library
      add_executable(static_demo {your test sources file names})
      target_link_libraries(static_demo SAntonaStandard)
      ```



#### Compile the Unit Tests

- Base on `Google Test` framework

1.  Enter the directory : `testing_set` 

   **Linux(Ubuntu 22.04)**: 

   - Enter  `build1` 

     ```bash
     cmake ..
     make
     ```

   - Leave from `build1` directory and enter  `CoverageStatistic` ，execute bash shell : `CreateFromDirBuild1.sh`  which will execute the testing programs automatically.

     ```bash
     ./CreateFromDirBuild1.sh
     ```

   - The coverage report will be outputted into `result` 

   

   **Windows(10)**:

   - 进入目录 `build` 

     ```powershell
     cmake .. -G 'MinGW Makefiles'
     mingw32-make
     ```

   - 测试程序生成路径为 `testing_set/bin/Windows`

#### History Versions

2022/12/30 AntonaStandard-v-1.0.0 Add file **Exception.h, Delegate.h** Details in AntonaStandard_introduction

2023/1/1     AntonaStandard-v-1.0.1 Update **Delegate.h** Details in AntonaStandard_introduction

2023/1/1	 AntonaStandard-v-1.1.0 Update **Delegate.h** Details in AntonaStandard_introduction

2023/1/2	 AntonaStandard-v-2.0.0 Add file **Refelction.h** Details in AntonaStandard_introduction

2023/1/11   AntonaStandard-v-3.0.0 Add file **Antona_foreach.h** Details in AntonaStandard_introduction

2023/2/19   AntonaStandard-v-4.0.0 Add file **AutoNode.h,AutoNode.cpp** Details in AntonaStandard_introduction

2023/2/25   AntonaStandard-v-4.1.0 Update **AutoNode.h,Exception.h**,Delete **AutoNode.cpp**

2023/2/27   AntonaStandard-v-5.0.0  Create a new namespace called AntonaMath,It will include severials math tools . Add file in to AntonaMath: **Fraction.h,Fraction.cpp** 

2023/3/31   AntonaStandard-v-6.0.0 Give a way to realize RTTI and related disposal——Filter,Add Files **Filter.h** 

2023/4/11   AntonaStandard-v-7.0.0  Create Thread concurrent tools relied on C++20 libraries. The Tread concurrent tools include **Sem_Extension** so far, it based on C++20 semaphore, you can realize **And semaphore**  **Semaphore Set** by using it appropriately.

2023/7/9    AntonaStandard-v-7.1.0 Perfect the cmake building files, it can build different kind of libraries and excutable file according to different platform. There are also several modification of project Reflection,Filter,Sem_Extension.

2023/8/8   AntonaStandard-v-8.0.0 Modified plenty of `namespace` s and the structure of directory `include` . At the mean time, added Threads pool .

2023/8/11 AntonaStandard-v-8.0.1 Add `.gitignore` to let **git tools** ignore the directories `bin` and `build` also `lib` (It means this project will not provide libraries that compiled from v-8.0.1 , but you can still make them by using cmake and make commands)

2023/8/17 AntonaStandard-v-9.0.0 Update file `.gitignore` , add new directories which should be ignored by git. Add cross platform (multi platform) support (AntonaStandard provides cross platform api for explicitly link (hot-loading) dynamic library.Update project Exception and optimize the directories structure. 

2023/9/15 AntonaStandard-v-9.1.0-Beta Update file `.gitignore` . Add cross platform support : An cross platform TCP/IPV4 Socket Communication Solution. In addition `AntonaStandard-v-9.1.0-Beta` is a testing version that means the version may be modified a lot in next version

2023/9/15 AntonaStandard-v-9.1.0 I redesigned the class to package IP address and Port better. Now this project provides an easy and usable cross platform **TCP/IPV4** and **TCP/IPV6** Socket communication solution.

2023/10/24 AntonaStandard-v-9.2.0 Transient version

2023/11/10 AntonaStandard-v-10.0.0 Import Unit test based on `Google Test` framework to promise the robustness. Replaced the raw pointer by smart pointer (`std::shared_ptr`), decrease the possibilities of memory leak. 

2023/12/12 AntonaStandard-v-10.0.1 Finished the fundamental functions of UDP Communications in `MultiPlatformSupport::SocketSupport`

#### Author's Blog

- [CSDN_Blog](https://blog.csdn.net/yyy11280335?spm=1000.2115.3001.5343) 