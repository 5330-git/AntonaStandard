#include <iostream>
#include "Reflection.h" 
using namespace std;
using namespace AntonaStandard;

class A{
public:
    A(){
        cout<<"A构造了"<<endl;
    };
};
class B{
public:
    B(){
        cout<<"B构造了"<<endl;
    };
};

// 派生反射类
class Factory:public Reflection{
public:
    Factory():Reflection(){this->load();};
    virtual void load()override{
        INIT;							// 初始化声明零时变量的宏
        REGISTER(A);					// 注册类A
        REGISTER(B);					// 注册类B
    }
};

int main(){
    Factory fac;							// 实例化派生的反射类（其实是一种广义的简单工厂）
    A* a  = (A*)fac.createInstance("A");	// 创建A的实例，注意需要对返回值类型进行强制转换
    B* b = (B*)fac.createInstance("B");
    fac.createInstance("C");				// 由于类C未被注册，因此会抛出异常
/* 输出
A构造了
B构造了
terminate called after throwing an instance of 'AntonaStandard::NotFound_Error'
  what():  The type you create was not registered!: C
*/
}