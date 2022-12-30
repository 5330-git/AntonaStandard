# Delegate事件委托类库

## 目录

[toc]

## 项目版本

| 版本号  | 版本描述                                                     | 时间       |
| ------- | ------------------------------------------------------------ | ---------- |
| v-1.0.0 | 完成事件委托基本功能可以满足多参数函数的委托                 | 2022/12/29 |
| v-1.0.1 | 修改createNew和getSelf的命名为copy和clone使之更加满足原型模式的命名 | 2022/12/30 |
| v-1.0.2 | 修复删除委托(-=)中未找到目标时传入的参数内存泄漏的问题       | 2022/12/30 |



## 项目目的

- 实现类似C#中的事件委托机制，设置一个可以绑定多个函数同时执行的类或函数（单线程内）
- 暂时不考虑事件委托的返回值，因为在C++中存储类型的引用是一个问题，如果事件委托添加的函数返回值是引用，在现有体系下会导致错误
- 参考链接：[(二)_YzlCoder的博客-CSDN博客_c++委托模式](https://blog.csdn.net/y1196645376/article/details/51416043)  
  - 项目的主体与参考链接基本相同，不过参考的代码存在一些问题，可以参考我在参考链接的文章中的评论，另外我用了一种比较巧妙的方式实现了复制构造函数
- 详细的介绍也可以看看我的博客
  - [【项目三】C++实现事件委托_学艺不精的Антон的博客-CSDN博客](https://blog.csdn.net/yyy11280335/article/details/128488238?spm=1001.2014.3001.5501) 



## 项目原理

- 使用多态和函数指针，分别封装普通函数，静态函数（普通函数和静态函数的函数指针属性相同，可以共用一个封装类）和成员函数的函数指针，最后通过Delegate类通过多态存储不同类型的指针
  - 为了能删除对应的函数指针，应当添加一个比较接口
- 通过可变模板参数可以实现多参数的函数的委托
- 利用**原型模式**，使得函数指针容器的复制不受具体类型的限制

## 项目依赖

- C++11标准
- AntonaStandard::NotFound_Error （#include "Exception.h"） 
- std::type_info
- std::list

## 平台（参考）

- Windows10
- VSCode
- GCC 11.2.0 x86_64-w64-mingw32

## 项目结构

- 我用abstract表示纯虚函数，用virtual 表示一般的虚函数，override表示重写

```mermaid
classDiagram
class BaseFuncPointerContainer~type_RETURN_TYPE,type_PARAMETERS_TYPE...~{
	+ abstract bool isType(const std::type_info& type)
	+ abstract type_RETURN_VALUE call(type_PARAMETERS_PACK... para_args)
	+ abstract bool equal(BaseFuncPointerContainer<type_RETURN_VALUE,type_PARAMETERS_PACK...>*container_ptr)
	+ virtual BaseFuncPointerContainer* clone()
	+ abstract BaseFuncPointerContainer* copy(const BaseFuncPointerContainer<type_RETURN_VALUE,type_PARAMETERS_PACK...>& other)
}

class Static_NarmalFuncPointerContainer~type_RETURN_TYPE,type_PARAMETERS_TYPE...~{
	# func_ptr	:函数指针
	+ override isType()
	+ override call()
	+ override equal()
	+ override clone()
	+ override copy()
}
class MethodFuncPointerContainer~type_RETURN_TYPE,type_PARAMETERS_TYPE...~{
	# func_ptr	:函数指针
	# obj_ptr	:对象指针
	+ override isType()
	+ override call()
	+ override equal()
	+ override clone()
	+ override copy()
}

Static_NarmalFuncPointerContainer--|>BaseFuncPointerContainer
MethodFuncPointerContainer--|>BaseFuncPointerContainer

class Delegate~type_RETURN_TYPE,type_PARAMETERS_TYPE...~{
	# list delegatelist
	+ virtual clear()
	+ virtual Delegate& operator+=()
	+ virtual Delegate& operator-=()
	+ virtual bool empty()
	+ virtual unsigned long size()
}
Delegate o--> BaseFuncPointerContainer

```

---

## AntonaStandard相关文件

| 文件名      | 内容               |
| ----------- | ------------------ |
| Exception.h | 常用异常类         |
| Delegate.h  | 事件委托的泛型实现 |

## 相关演示

- 创建事件委托

```cpp
// 注意引入头文件#include "Delegate"
	// 创建一个接收无返回值无参数的函数的事件委托
using namespace AntonaStandard;
Delegate<void> v_v_del;					// 如果无参数请不要在参数模板中添加void,并且，模板参数必须包含一个返回值参数
	// 创建一个接收int返回值无参数的事件委托
Delegate<int,int> i_i_del;					// 当前版本暂时不支持对有返回值的函数的返回值存储功能，因此所有的返回值将被丢弃
	// 创建一个接收int返回值无参数的事件委托
Delegate<int> i_v_del;
	// 创建一个接收void返回值，int引用的事件委托
Delegate<void,int&> v_iq_del;
	// 创建一个int&返回值，int&参数的事件委托
Delegate<int&,int&> iq_iq_del;
```

- 声明函数

```cpp
// 声明如下两个类
class A{
public:
    void m_func_a_v_v(){
        cout<<"A的非静态无返回值无参数成员函数"<<endl;
    }
    static void s_m_func_a_v_v(){
        cout<<"A的静态无返回值无参数成员函数"<<endl;
    }
    int& m_func_a_iq_iq(int& value){
        cout<<"A的非静态int&返回值int&参数成员函数 "<<value<<endl;
        return value;
    }
};
class B{
public:
    void m_func_b_v_v(){
        cout<<"B的非静态无返回值无参数成员函数"<<endl;
    }
    static int& s_m_func_b_iq_iq(int& value){
        cout<<"B的静态int&返回值int&参数成员函数 "<<value<<endl;
        return value;
    }
}
// 声明两个普通函数
void n_func_v_v(){
    cout<<"无返回值无参数普通函数"<<endl;
}
int& n_func_iq_iq(int& value){
    cout<<"int&返回值int&参数普通函数 "<<value<<endl;
    return value;
}
```

- 添加委托

```cpp
// 创建类
A a;
B b;

v_v_del += newDelegate(n_func_v_v);			// 添加无返回，无参数普通函数
v_v_del += newDelegate(a,a.m_func_a_v_v);	// 添加无返回，无参数a的非静态成员函数
v_v_del += newDelegate(b,b.m_func_b_v_v);	// 添加无返回，无参数b的非静态成员函数
v_v_del += newDelegate(A::s_m_func_a_v_v);	// 添加无返回，无参数a的静态成员函数

iq_iq_del += newDelegate(n_func_iq_iq);			// int&返回值int&参数的普通函数
iq_iq_del += newDelegate(a,a.m_func_a_iq_iq);	// int&返回值int&参数a的非静态成员函数
iq_iq_del += newDelegate(B::s_m_func_b_iq_iq);	// int&返回值int&参数b的静态成员函数

// 实例对象的位置也可以写成实例对象的指针(有对应的重载版本)
// v_v_del += newDelegate(&a,a.m_func_a_v_v);
```

- 调用委托

```cpp
v_v_del();
/*输出
c无返回值无参数普通函数
A的非静态无返回值无参数成员函数
B的非静态无返回值无参数成员函数
A的静态无返回值无参数成员函数
*/

int value = 666;
iq_iq_del(value);
/*
int&返回值int&参数普通函数 666
A的非静态int&返回值int&参数成员函数 666
B的静态int&返回值int&参数成员函数 666
*/
```

- 删除委托

```cpp
v_v_del -= newDelegate(n_func_v_v);
v_v_del -= newDelegate(a,a.m_func_a_v_v);

// 如果尝试删除不存在的委托，会抛出异常(AntonaStandard::NotFound_Error)
```

- 查看委托个数,判断委托是否为空，清除所有委托

```cpp
// 查看委托个数
v_v_del.size()
    
// 判断委托是否为空
v_v_del.empty()

// 清除所有委托
v_v_del.clear()
```

- 左值引用复制构造

```cpp
Delegate<void> v_v_del_1 = v_v_del;
```

- 右值引用移动构造

```cpp
Delegate<void> v_v_del_2 = std::move(v_v_del);		// 将v_v_del转化成右值引用，这样可以调用内部的list的右值移动构造函数，直接将v_v_del的链表管理权限交给v_v_del_2防止额外开辟内存，v_v_del变成将亡值
```

