#include <iostream>
#include <queue>
#include "Utilities/AutoNode.h"
#include "MultiPlatformSupport/MultiPlatformMacro.h"
using namespace std;
using namespace AntonaStandard::Utilities;
template<typename type_Key>
class TreeNode:public AutoNode<type_Key>{
public:
    TreeNode(const type_Key& k){this->key = k;};
    virtual AutoNode<type_Key>* clone()override{
        return new TreeNode(this->key);
    }
    virtual ~TreeNode()override{
        for(auto& i:(*this)){
            delete i.second;
            i.second = nullptr;
        }
    }
};
template<typename type_Key>
class GreenNode:public TreeNode<type_Key>{
public:
    GreenNode(const type_Key& k):TreeNode<type_Key>::TreeNode(k){};
    virtual AutoNode<type_Key>* clone()override{
        return new GreenNode(this->key);
    }
    void outPutGreenNodeMessage(){
        cout<<" GreenNode: ";
    }
};
template<typename type_Key>
class YellowNode:public TreeNode<type_Key>{
public:
    YellowNode(const type_Key& k):TreeNode<type_Key>::TreeNode(k){};
    virtual AutoNode<type_Key>* clone()override{
        return new YellowNode(this->key);
    }
    void outPutYellowNodeMessage(){
        cout<<" YellowNode: ";
    }
};
template<typename type_Key>
class RedNode:public TreeNode<type_Key>{
public:
    RedNode(const type_Key& k):TreeNode<type_Key>::TreeNode(k){};
    virtual AutoNode<type_Key>* clone()override{
        return new RedNode(this->key);
    }
    void outPutRedNodeMessage(){
        cout<<"RedNode: ";
    }
};
template<typename type_Key>
void search(AutoNode<type_Key>* root){
    // 广度优先遍历
    queue<AutoNode<type_Key>*> q;
    q.push(root);
    while(!q.empty()){
        for(auto& i:*(q.front())){
            q.push(i.second);
        }
        // 使用dynamic_cast转换来判断实例的类型，然后调用对应的函数
        if(auto p = dynamic_cast<RedNode<type_Key>*>(q.front())){
            p->outPutRedNodeMessage();
        }
        else if(auto p = dynamic_cast<GreenNode<type_Key>*>(q.front())){
            p->outPutGreenNodeMessage();
        }
        else if(auto p = dynamic_cast<YellowNode<type_Key>*>(q.front())){
            p->outPutYellowNodeMessage();
        }
            cout<<q.front()->getKey()<<" ";
            q.pop();
        }
        cout<<endl;
}
int main(){
    TreeNode<const char*>* root = new TreeNode<const char*>("Root");
    root->add("A1",new GreenNode<const char*>("A1"));
    root->add("A2",new YellowNode<const char*>("A2"));
    (*root)["A1"].add("B1",new RedNode<const char*>("B1"));
    (*root)["A1"].add("B2",new YellowNode<const char*>("B2"));
    (*root)["A2"].add("B3",new GreenNode<const char*>("B3"));
    (*root)["A2"].add("B4",new RedNode<const char*>("B4"));
    search(root);
    auto r1 = root->copy();
    search(r1);
    auto r2 = (*root)["A1"].copy();
    search(r2);
    delete root;
    delete r1;
    delete r2;
    #ifdef AntonaStandard_PLATFORM_WINDOWS
        system("pause");
    #endif
    return 0;
}