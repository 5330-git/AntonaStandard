# AntonaStandard

#### Introduction

**Brief** 

A basic C++ library aimed at learning the fundamental principles of C++ and CMake building. So far, it supports mathematical fractions, basic reflection mechanisms, basic delegation mechanisms, cross-platform dynamic library hot loading, cross-platform socket libraries, and high-level encapsulation of TCP-UDP sockets that support both ipv4 and ipv6. And it supports generating documents by Doxygen automatically!   Additionally, it supports the Configure mode of CMake ’s `find_package`. Furthermore, it contains unit tests based on CTest and Google Test. The only drawback is the lack of unit test code to ensure the quality of the overall code.😉🙌

**Submodules** 

The submodules are included in directory `third_party` . you can find more details in the file `.gitmordules` 

- **Google Test** 
  - Our project based on `1.14.x` Version of Google Test to realize unit test. And Google Test is a submodule , you can set the cmake option `-DBUILD_TESTS=ON` to enable Google Test and unit tests. The Google Test libraries will be install in the same directory where `AntonaStandard` is installed. 

#### Documents

##### View On line Docs

- link : 



##### Generate Docs locally

- You can use the following command to generate documents. 

  ```bash
  # change your workspace to the top directory of AntonaStandard
  doxygen Doxyfile
  ```

- The html files of documents will be generated into `docs/Doxygen/html` . You can find an index html file `index.html` . Find it and open it in your browser. 

#### Clone the repository

- you need add `--recurse-submodule` option to clone the depend submodules

  ```bash
  git clone --branch master --recurse-submodule https://gitee.com/ordinaryAnton/antona-standard.git AntonaStandard
  ```

- or you can input the following command after clone the `AntonaStandard` 

  ```bash
  git submodule update --recursive --remote
  ```

  

#### Installation

- Main directory of `AntonaStandard` :

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

- create a directory by `mkdir` to build the project

  ```bash
  mkdir buildlin
  cd buildlin
  ```

- run `cmake` 

  ```bash
  cmake .. -DBUILD_EXAMPLES=ON -DBUILD_TESTS=ON -DCMAKE_INSTALL_PREFIX=<path to install>
  ```

  - `-DBUILD_EXAMPLES=ON`  : the option will enable the examples , and the example executable will generated in the `bin` of correspond component directory.
  - `-DBUILD_TESTS=ON` : the option will enable the unit tests, and Google Test libraries will be compiled at the same time.
  - `-DCMAKE_INSTALL_PREFIX=<path to install` : Setting the directory to install `AntonaStandard` and it's submodule

- run `make` 

  ```bash
  make -j 16
  ```

- run unit tests

  ```bash
  make test
  ```

- install

  ```bash
  make install
  ```



- uninstall

  ```bash
  make uninstall
  ```

  

**Windows(10)**:

- Firstly, open shell tool: `PowerShell`

- create a directory by `mkdir` to build the project

  ```bash
  mkdir buildlin
  cd buildlin
  ```

- run `cmake` 


- ```bash
  cmake .. -DBUILD_EXAMPLES=ON -DBUILD_TESTS=ON -DCMAKE_INSTALL_PREFIX=<path to install>
  ```

  - `-G 'MinGW Makefiles'`  : set the building system as  Mingw
  - `-DBUILD_EXAMPLES=ON`  : the option will enable the examples , and the example executable will generated in the `bin` of correspond component directory.
  - `-DBUILD_TESTS=ON` : the option will enable the unit tests, and Google Test libraries will be compiled at the same time.
  - `-DCMAKE_INSTALL_PREFIX=<path to install` : Setting the directory to install `AntonaStandard` and it's submodule

- run `make` 

  ```bash
  mingw32-make -j 16
  ```

- run unit tests

  ```bash
  mingw32-make test
  ```

- install

  ```bash
  mingw32-make install
  ```



- uninstall

  ```bash
  mingw32-make uninstall
  ```

  

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

2024/2/25 AntonaStandard-v-11.0.0 Complete the support for cmake module, complete the automatic support for CTest and Google Test, and complete the TCP-UDP socket advanced encapsulation (Network component) for ipv4 and ipv6.

#### Author's Blog

- [CSDN_Blog](https://blog.csdn.net/yyy11280335?spm=1000.2115.3001.5343) 