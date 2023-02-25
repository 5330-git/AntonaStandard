#ifndef AUTONODE_H
#define AUTONODE_H

#include <map>
#include <stdexcept>
#include "Exception.h"
#include <sstream>

#define AntonaStandard_AutoNode_VERSION "1.0.0"
#define AntonaStandard_AutoNode_EDIT_TIME "2023/2/19"
#define AntonaStandard_AutoNode_AUTHOR "Anton"

/*
*   Decoded by utf-8
*   v-1.0.0 2023/2/19 - 初步实现自动节点，
*   v-1.1.0 2023/2/25 - 修改自动结点的邻居存储方式，添加clone函数实现浅复制
*
*/

// 前置声明
namespace AntonaStandard{
    // class AutoNode;         // 抽象类，做接口使用
    template<typename type_Key>class AutoNode;
    
}
// 类的结构声明和定义
namespace AntonaStandard{
    template<typename type_Key>
    class AutoNode{
    protected:
        std::map<type_Key,AutoNode*> neighbors;
        type_Key key;
    public:
        using iterator = typename std::map<type_Key,AutoNode*>::iterator;
        virtual ~AutoNode(){};
        virtual AutoNode* clone()=0;
        virtual AutoNode* copy(){
            AutoNode<type_Key>* root_ptr = this->clone();
            if(this->empty()){
                return root_ptr;
            }
            for(auto& i:(*this)){
                root_ptr->add(i.first,i.second->copy());
            }
            return root_ptr;
        }
        virtual AutoNode* remove(const type_Key& k){
            AutoNode::iterator iter = this->neighbors.find(k);
            if(iter == this->end()){
                return nullptr;
            }
            else{
                this->neighbors.erase(iter);
                return iter->second;
            }
        }
        virtual void add(type_Key&& k,AutoNode* node_ptr){
            this->add(k,node_ptr);
        }
        virtual void add(const type_Key& k,AutoNode* node_ptr){
            if(node_ptr == nullptr){
                throw NullPointer_Error("Got Null Pointer in function AutoNode::add(~~)");
            }
            AutoNode::iterator iter = this->neighbors.find(k);
            if(iter == this->neighbors.end()){
                // 无冲突
                this->neighbors.insert(std::pair<type_Key,AutoNode*>(k,node_ptr));
            }
            else{
                // 有冲突
                throw Conflict_Error("There is a conflict when using function AntoNode::add(~~) because of an existed key!");
            }
        }
        inline virtual AutoNode& operator[](const type_Key& k){
            AutoNode::iterator iter = this->neighbors.find(k);
            if(iter != this->neighbors.end()){
                if(iter->second){
                    return *(iter->second);
                }
                else{
                    throw AntonaStandard::NullPointer_Error("Got Null Pointer in AutoNode::operator[](~~)");
                }
            }
            else{
                throw AntonaStandard::NotFound_Error("The Key was not found! In Function AutoNode::operator[](~~)");
            }
        }
        inline virtual AutoNode* visit(const type_Key& k){
            AutoNode::iterator iter = this->neighbors.find(k);
            if(iter != this->neighbors.end()){
                return iter->second;
            }
            else{
                throw AntonaStandard::NotFound_Error("The Key was not found! In Function AutoNode::operator[](~~)");
            }
        }
        inline AutoNode::iterator begin(){
            return this->neighbors.begin();
        }
        inline AutoNode::iterator end(){
            return this->neighbors.end();
        }
        inline virtual bool empty(){
            return this->neighbors.empty();
        }
        inline virtual size_t size(){
            return this->neighbors.size();
        }
        inline virtual const type_Key& getKey()const{
            return this->key;
        }
    };
}
#endif