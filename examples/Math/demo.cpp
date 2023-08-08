#include <iostream>
#include "Math/Fraction.h"   // 分数库
using namespace std;
using namespace AntonaStandard::Math;
int main(){
    cout<<"分数库演示(Demo of Library Fraction):"<<endl;
    // 无参构造
    Fraction f;						// f == "0/1"
    // 单int构造
    Fraction f1(10);				// f1 == "10/1"
    // 双int构造
    Fraction f2(12,3);				// f2 == "12/3"
    // 单Fraction左值构造
    Fraction f3(f2);				// f3 == "12/3"
    // 单Fraction右值购造
    Fraction f4(std::move(f3));		// f4 == "12/3"
    // 单const char* 构造
    Fraction f5("24/5");			// f5 == "24/5"
    Fraction f6 = "24/5";			// f6 == "24/5"

    cout<<f1+"1/4"<<endl;					// 6/1
    cout<<f2-"1/2"<<endl;					// 35/1
    cout<<f1+4<<endl;						// 39/4	
    cout<<60-f2<<endl;						// 49/2
    cout<<(f1+="1/4")<<endl;				// 6/1
    cout<<(f2-="1/2")<<endl;				// 35/1

    cout<<(f1==f2)<<endl;					// 0
    cout<<(f1>f2)<<endl;					// 0
    cout<<(f1>="6")<<endl;					// 1
    cout<<(f2<="35/1")<<endl;				// 1
    cout<<(f1<18)<<endl;					// 1
    cout<<(f2!=35)<<endl;					// 0

    Fraction f7 = -1*f1 ;
    cout<<(f7 == -f1)<<endl;					// 1
    // 输入流运算符
    Fraction f8;
    cin>>f8;								// 会尝试按照"num/den"的形式读取，如果不存在字符'/'则会终止读取den，并将den赋值为1
    cout<<f8<<endl;
    return 0;
}