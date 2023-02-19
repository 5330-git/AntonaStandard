#include "AutoNode.h"
namespace AntonaStandard{
    AutoNode* AutoNode::remove(size_t index){
        // 溢出检查
        if(index<0||index >= this->neighbors.size()){
            std::stringstream out_exp;
            out_exp<<"Unlawful index: "<<index<<" out of range [0,"<<this->neighbors.size()-1<<"]";
            throw std::overflow_error(out_exp.str().c_str());
        }
        // 用末尾保存的指针覆盖index
        AutoNode* removed_ptr = this->neighbors[index];
        this->neighbors[index] = this->neighbors.back();
        this->neighbors.pop_back();
        return removed_ptr;
    }

    void AutoNode::remove(AutoNode& node){
        AutoNode* node_ptr = &node;
        // 通过线性查找的办法定位
        for(int i = 0;i<this->neighbors.size();++i){
            if(this->neighbors[i] == node_ptr){
                AutoNode* ret = this->remove(i);
                return;
            }
        }
        // 循环正常结束，表示没有找到目标值
        std::stringstream out_exp;
        out_exp<<"Member not found : "<<node_ptr;
        throw AntonaStandard::NotFound_Error(out_exp.str().c_str());

    }
    void AutoNode::add(AutoNode* node_ptr){
        // 检查是否是空指针
        if(node_ptr == nullptr){
            std::stringstream out_exp;
            out_exp<<"Attempt to add Null Pointer in function \"void AutoNode::add(AutoNode*)\"";
            throw AntonaStandard::NullPointer_Error(out_exp.str().c_str());
        }
        this->neighbors.push_back(node_ptr);
    }
}