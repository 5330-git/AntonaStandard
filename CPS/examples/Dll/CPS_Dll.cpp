#include <iostream>
#include <CPS/Macros.h>
#include <CPS/Dll.h>
#include <string>
using namespace std;
using namespace AntonaStandard::CPS;

int main(){
    using calc_t = double(*)(double,double);
    using sayHello_t = void(*)();
    cout<<"Please input the absolute path of demolib"<<endl;
    string path;
    cin>>path;
    // 加载动态库
    HandlePtr_t handle = loadDll(path);
    // 获取动态库中的函数地址
    sayHello_t dll_sayHello = (sayHello_t)getFunc("sayHello",handle);
    calc_t dll_add = (calc_t)getFunc("add_db",handle);
    calc_t dll_sub = (calc_t)getFunc("sub_db",handle);
    calc_t dll_multi = (calc_t)getFunc("multi_db",handle);
    calc_t dll_div = (calc_t)getFunc("div_db",handle);

    // 调用函数
    dll_sayHello();
    cout<<"Please Input 2 numbers to caculate"<<endl;
    double a,b;
    cin>>a>>b;
    cout<<a<<"+"<<b<<"="<<dll_add(a,b)<<endl;
    cout<<a<<"-"<<b<<"="<<dll_sub(a,b)<<endl;
    cout<<a<<"*"<<b<<"="<<dll_multi(a,b)<<endl;
    cout<<a<<"/"<<b<<"="<<dll_div(a,b)<<endl;

    // 卸载动态库句柄
    unloadDll(handle);
    cout<<"successfully unload dll"<<endl;
    #ifdef AntonaStandard_PLATFORM_WINDOWS
        system("pause");
    #endif
    return 0;

}
