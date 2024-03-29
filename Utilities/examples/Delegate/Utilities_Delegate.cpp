#include <iostream>
#include <Utilities/Delegate.h>
#include <CPS/Macros.h>
using namespace std;
using namespace AntonaStandard::Utilities;

class A{
public:
    static int s_i_v_A(){
        cout<<"non-member static function of class A: \'int(*)()\' "<<endl;
        return 10;
    }
    int i_v_A(){
        cout<<"member function of class A: \'int (*)()\' "<<endl;
        return 11;
    }

};
int i_v(){
    cout<<"non-member function : \'int(*)()\' "<<endl;
    return 12;
}
class B{
public:
    static void s_v_v_B(){
        cout<<"non-member static function of class B: \'void(*)()\' "<<endl;
    }
    
    void v_v_B(){
        cout<<"member function of class B: \'void(*)()\' "<<endl;
    }
};
void v_v(){
    cout<<"non-member function : \'void(*)()\' "<<endl;
}

class C{
public:
    static int& s_iq_iq_C(int& v){
        cout<<"non-member static function of class C: \'int&(*)(int&)\' "<<v<<endl;
        return v;
    }
    int& iq_iq_C(int& v){
        cout<<"member function of class C: \'int&(*)(int&)\' "<<v<<endl;
        return v;
    }
};
int& iq_iq(int& v){
    cout<<"non-member function of class C: \'int&(*)(int&)\' "<<v<<endl;
    return v;
}

int main(){
    A a;
    B b;
    C c;

    // 实例化委托类
    Delegate<int(void)> del;
    Delegate<void()> del1;
    Delegate<int&(int&)> del2;

    // 添加委托
    del += newDelegate(A::s_i_v_A);
    del += newDelegate(a,&A::i_v_A);         // 对于写法： del += newDelegate(a,a.i_v_A); 在Linux环境下会报错，这里同一写成现在的样子
    del += newDelegate(i_v);

    del1 += newDelegate(B::s_v_v_B);
    del1 += newDelegate(b,&B::v_v_B);
    del1 += newDelegate(v_v);

    del2 += newDelegate(C::s_iq_iq_C);
    del2 += newDelegate(c,&C::iq_iq_C);
    del2 += newDelegate(iq_iq);
    // 调用委托
    vector<int> vec = del();			// 返回值为int，无参数列表调用演示
    cout<<"--------------"<<endl;
    del.call_without_return();			// 无返回值调用办法
    for(int i = 0;i<vec.size();++i){
        cout<<vec[i]<<" ";				// 查看保存的返回值
    }
    cout<<endl<<"--------------"<<endl;
    del1();								// 无返回值，无参数调用演示
    cout<<endl<<"--------------"<<endl;
    int x = 666;
    vector<int> vec2 = del2(x);			// 返回值为int,int&参数调用演示
    for(int i = 0;i<vec2.size();++i){
        cout<<vec2[i]<<" ";
    }
    cout<<endl;
    #ifdef AntonaStandard_PLATFORM_WINDOWS
        system("pause");
    #endif
    return 0;
}
