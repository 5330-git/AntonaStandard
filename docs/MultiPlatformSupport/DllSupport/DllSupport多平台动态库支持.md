# DllSupport 多平台动态库支持

## 目录

[toc]



## 项目版本

| 版本号  | 版本描述                                           | 时间      |
| ------- | -------------------------------------------------- | --------- |
| v-1.0.0 | 初步实现：提供跨平台的动态库显式链接（热加载）支持 | 2023/8/17 |

## 项目目的

- 提供跨平台的（支持Windows和Linux平台）动态库热加载（显式链接）支持。



## 函数文档

### `HandlePtr_t`

- 分别封装了不同平台的句柄类型
  - Windows: `HMODULE` 
  - Linux: `void*` 



### `HandlePtr_t loadDll(const char* )`

- 内部封装了不同平台下的**动态库显式链接** API，用于加载path路径下的动态库
- Windows平台下封装的是：
  - `LoadLibraryA` 
  - 通过 `GetLastError` 获取错误代码
- Linux平台下封装的是：
  - `dlopen` 
  - 通过 `dlerror` 获取错误信息
- 可能抛出异常：
  - `AntonaStandard::Utilities::FailToLoadDll_Error` 

- 其它重载版本：

  `HandlePtr_t loadDll(const std::string& )` 

  

### `void* getFunc(const char* ,HandlePtr_t)`

- 内部封装了不同平台下的 API，用于获取动态库内指定名称的函数的函数地址
- Windows平台下封装的是：
  - `GetProcAddress` 
  - 通过 `GetLastError` 获取错误代码
- Linux平台下封装的是：
  - `dlsym` 
  - 通过 `dlerror` 获取错误信息
- 可能抛出的异常：
  - `AntonaStandard::Utilities::FailToGetFunction_Error` 
- 其它重载版本：
  - `void* getFunc(const std::string& ,HandlePtr_t)` 



### `void unloadDll(HandlePtr_t)` 

- 内部封装了不同平台下的 API， 用于释放对应的动态库句柄
- Windows平台下封装的是：
  - `FreeLibrary` 
  - 通过 `GetLastError` 获取错误代码
- Linux 平台下封装的是：
  - `dlclose` 
  - 通过 `dlerror` 获取错误信息

## 注意事项

- ==使用显示链接的动态库的函数必须使用AntonaStandard提供的跨平台导出宏`ASD_EXPORT_FUNC`进行修饰，同时为了防止C++下对函数的修饰必须采用C语言的方式编译（使用`extern "C"` 关键字修饰）== 