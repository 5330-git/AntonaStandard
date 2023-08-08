# Exception异常类库

## 目录

[toc]

## 项目版本

| 版本号  | 版本描述                                                     | 时间       |
| ------- | ------------------------------------------------------------ | ---------- |
| v-1.0.0 | 初步实现                                                     | 2022/12/19 |
| v-1.1.0 | 添加NotFound_Error 用于标记未遍历到目标值的错误              | 2022/12/19 |
| v-1.1.1 | 更改项目宏信息                                               | 2022/1/2   |
| v-1.2.0 | 添加Conflict_Error 用于处理系统中出现的冲突错误              | 20222/24   |
| v-1.3.0 | 修改命名空间从 `AntonaStandard` 到 `AntonaStandard::Utilities` | 2023/8/8   |



## 项目目的

- stdexcpt中的常用派生异常类不足以表示现实开发中的个中问题，并且如果每次开发都单独派生出异常会出现管理混乱的情况，因此在此基于stdexcpt中的异常类进一步派生出更加详细的异常类，并将它们封装在一个源文件中，方便管理。

## 项目原理

- 基于C++类的派生实现

## 项目依赖

- C++11标准
- (#include \<stdexcept\>)

## 平台（参考）

- Windows10
- VSCode
- GCC 11.2.0 x86_64-w64-mingw32

## 项目结构

| 类名                    | 基类               | 描述                                                   |
| ----------------------- | ------------------ | ------------------------------------------------------ |
| Uninitialization_Error  | std::logic_error   | 未初始化异常，如果系统未初始化需要抛出该异常           |
| WrongArgument_Error     | std::logic_error   | 错误参数异常，当系统接收到了错误参数时                 |
| UnlawfulOperation_Error | std::logic_error   | 非法操作异常，一般由用户的非法操作触发                 |
| Unkown_Error            | std::logic_error   | 未知异常                                               |
| SystemCollapse_Error    | std::runtime_error | 系统崩溃异常                                           |
| NullPointer_Error       | std::logic_error   | 空指针异常，一般是尝试访问空指针时抛出                 |
| NotFound_Error          | std::logic_error   | 未找到异常，当遍历查找一个目标值但未找到可以抛出该异常 |
| Conflict_Error          | std::logic_error   | 当尝试向一个序列中插入已有的记录时，可能会触发该异常   |



## AntonaStandard相关文件

| 文件名      | 内容       |
| ----------- | ---------- |
| Exception.h | 常用异常类 |

## 相关演示

- 抛出异常演示

```cpp
// 以空指针异常为例
try{
    int a = nullptr;
    if(a == nullptr){
        throw AntonaStandard::NullPointer_Error("接收到一个空指针");
	}
    // 未触发异常时的操作
    // ......
    // ......
}
catch(AntonaStandard::NullPointer_Error& e){
    // 异常处理
}

// 当然和std中的异常一样，可以由相关的函数抛出
void func(参数){
    if(某种条件){
        throw AntonaStandard::Unkonwn_Error("在函数void func(参数)中触发了一个未知操作");
    }
}

// 当然，如果没有catch来接收相关异常，终端会接收并显示出来，前提是这个异常得派生自std::exception
```
