#include <iostream>
#include "Utilities/TypeFilter.h" 
using namespace std;
using namespace AntonaStandard::Utilities;

class Base{
public:
    Base(){};
    virtual void say(){
        cout<<"Base say hello: ";
    }
};

class Child1:public Base{
public:
    Child1(){};
    virtual void say()override{
        cout<<"Child1 say hello: ";
    }
    void Child1_work(){
        cout<<"Child1_work !"<<endl;
    }
};

class Child2:public Child1{
public:
    Child2(){};
    virtual void say()override{
        cout<<"Child2 say hello: ";
    }
    void Child2_work(){
        cout<<"Child2_work !"<<endl;
    }
};
class Child3:public Base{
public:
    Child3(){};
    virtual void say()override{
        cout<<"Child3 say hello: ";
    }
    void Child3_work(){
        cout<<"Child3_work !"<<endl;
    }
};
class Child4:public Child2{
public:
    Child4(){};
    virtual void say()override{
        cout<<"Child4 say hello: ";
    }
    void Child4_work(){
        cout<<"Child4_work !"<<endl;
    }
};

// 过滤器,用于处理分支 Child1,Child3
class BaseFilter1:public Filter<Base,Base>{
public:
    BaseFilter1(Basic_Filter* parent=nullptr):Filter(parent){};
    virtual bool dispose(Base* user,Base* base_ptr)override{
        if(typeid(*base_ptr) == typeid(Child1)){
            static_cast<Child1*>(base_ptr)->Child1_work();
            return true;
        }
        else if(typeid(*base_ptr) == typeid(Child3)){
            static_cast<Child3*>(base_ptr)->Child3_work();
            return true;
        }
        return this->Filter::dispose(user,base_ptr);
    }
    void test(){
        this->children.push_back(nullptr);
    }
};

// 过滤器，添加两种处理分支 Child2,Child4
class BaseFilter2:public Filter<Base,Base>{
public:
    BaseFilter2(Basic_Filter* p_=nullptr):Filter(p_){};
    virtual bool dispose(Base* user,Base* base_ptr)override{

        if(typeid(*base_ptr) == typeid(Child2)){
            static_cast<Child2*>(base_ptr)->Child2_work();
            return true;
        }
        else if(typeid(*base_ptr) == typeid(Child4)){
            static_cast<Child4*>(base_ptr)->Child4_work();
            return true;
        }
        return this->Filter::dispose(user,base_ptr);
    }
};
int main(){
    vector<Base*> vec;
    
    BaseFilter1* filter1 = new BaseFilter1;

    BaseFilter2* filter2_ptr = new BaseFilter2(filter1);
    filter1->install(filter2_ptr);          // 为filter1安装过滤器filter2
    vec.push_back(new Child1);
    vec.push_back(new Child2);
    vec.push_back(new Child3);
    vec.push_back(new Child4);

    for(auto& i:vec){
        i->say();
        filter1->dispose(nullptr,i);		// 没有反馈对象，因此user_ptr标记为nullptr
    }
    cout<<endl<<filter1->remove(nullptr)<<endl;
    filter1->remove(filter2_ptr);       // 卸载过滤器filter2，将不再对Child2和Child4起到过滤作用
    for(auto& i:vec){
        filter1->dispose(nullptr,i);
        delete i;
    }
    filter1->install(nullptr);          // 安装空指针会报错
    delete filter1;

}
/*
Child1 say hello: Child1_work !
Child2 say hello: Child2_work !
Child3 say hello: Child3_work !
Child4 say hello: Child4_work !

0
Child1_work !
Child3_work !
terminate called after throwing an instance of 'AntonaStandard::NullPointer_Error'
  what():  Got a null pointer in Filter!
*/