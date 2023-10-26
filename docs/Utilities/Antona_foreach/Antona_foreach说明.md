# Antona_foreach

## 目录

[toc] 

## 项目版本

| 版本号  | 版本描述                                                     | 时间      |
| ------- | ------------------------------------------------------------ | --------- |
| v-1.0.0 | 初步实现，添加了几个模板，允许使用base-range for syntax逆向迭代容器。 | 2023/1/11 |
| v-1.1.0 | 修改命名空间从 `AntonaStandard` 到 `AntonaStandard::Utilities` | 2023/8/8  |

## 项目目的

- 专门为迭代使用，初版的功能是为了实现基于base-range for syntax逆向迭代的功能
- 参考链接
  - https://stackoverflow.com/a/36928761/20203824 

## 项目原理

- C++模板函数显式具体化

## 项目依赖

- C++11标准

## 平台

- Windows10
- VSCode
- GCC 11.2.0 x86_64-w64-mingw32

## 项目结构

- 略

## AntonaStandard相关文件

| 文件名         | 内容                   |
| -------------- | ---------------------- |
| Antona_foreach | 与迭代器有关的模板函数 |

## 相关演示

```cpp
#include <iostream>
#include <vector>
#include "Antona_foreach.h"
using namespace std;
using namespace AntonaStadandard;
int main() {
    int a[] = {1,2,3};
    const char* s[] = {"hello","world!","!"};
    vector<int> b = {1,2,3};
    for(auto i:a){
        cout<<i<<" ";
    }
    cout<<endl;
    for(auto i:r_wrap(a)){
        cout<<i<<" ";
    }
    cout<<endl;
    for(auto i:r_wrap(b)){
        cout<<i<<" ";
    }
    cout<<endl;
    for(auto i:r_wrap(s)){
        cout<<i<<" ";
    }
	return 0;   
}
/*
1 2 3 
3 2 1
3 2 1
! world! hello
*/
```



## 注意事项

- 对于自定义的容器**必须**要有以下几个函数和类型
  - 获取反向首迭代器 rbegin()
  - 获取反向尾迭代器 rend()
  - 反向迭代器类型 reverse_iterator