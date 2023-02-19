#ifndef AUTONODE_H
#define AUTONODE_H

#include <vector>
#include <stdexcept>
#include "Exception.h"
#include "sstream"

#define AntonaStandard_AutoNode_VERSION "1.0.0"
#define AntonaStandard_AutoNode_EDIT_TIME "2023/2/19"
#define AntonaStandard_AutoNode_AUTHOR "Anton"

/*
*   Decoded by utf-8
*   v-1.0.0 2023/2/19 - 初步实现自动节点，
*
*
*/

// 前置声明
namespace AntonaStandard{
    class AutoNode;         // 抽象类，做接口使用
}
// 类的结构声明和定义
namespace AntonaStandard{
    class AutoNode{
    protected:
        std::vector<AutoNode*> neighbors;
    public:
        using iterator = typename std::vector<AutoNode*>::iterator;
        
        virtual ~AutoNode(){};          // 析构函数
        // virtual AutoNode* clone()=0;    // 原型模式，实现克隆（浅复制)
            // 无法通过基类指针调用来获取派生类类型的指针，因此废弃
        virtual AutoNode* copy()=0;       // 原型模式，实现拷贝（深复制）
        virtual AutoNode* remove(size_t index);
        virtual void remove(AutoNode& node);
        virtual void add(AutoNode* node_ptr);
        inline virtual AutoNode& operator[](size_t index){
            return *(this->neighbors[index]);
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
    };
}
#endif