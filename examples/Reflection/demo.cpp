#include <iostream>
#include "Utilities/Reflection.h" 
#include "MultiPlatformSupport/MultiPlatformMacro.h"
using namespace std;
using namespace AntonaStandard::Utilities;

class A{
public:
    A(){
        cout<<"Construct A!"<<endl;
    };
};
class B{
public:
    B(){
        cout<<"Construct B"<<endl;
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
    auto a  = std::static_pointer_cast<A>(fac.createInstance("A"));	// 创建A的实例，注意需要对返回值类型进行强制转换
    auto b = std::static_pointer_cast<B>(fac.createInstance("B"));
    try{
        fac.createInstance("C");				// 由于类C未被注册，因此会抛出异常
    }
    catch(AntonaStandard::Utilities::NotFound_Error& e){
        cout<<e.what()<<endl;
    }
    #ifdef AntonaStandard_PLATFORM_WINDOWS
        system("pause");
    #endif
    return 0;
}