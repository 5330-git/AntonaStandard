#ifndef FILTER_H
#define FILTER_H

#include <map>
#include <vector>
#include "Exception.h"

#define AntonaStandard_Filter_VERSION "1.2.0"
#define AntonaStandard_Filter_EDIT_TIME "2023/8/8"
#define AntonaStandard_Filter_AUTHOR "Anton"

/*
*   2023/3/31   v-1.0.0 初步实现过滤器
*   2023/7/8    v-1.1.0 为了防止与标准库Filter.h重名，修改头文件名称为 TypeFilter.h
*   2023/8/8    v-1.2.0  修改命名空间从 AntonaStandard 到 AntonaStandard::Utilities

*/
namespace AntonaStandard{
    namespace Utilities{
        class Basic_Filter;
        template<typename type_USER,typename type_IDENTI> class Filter;
    }
}

namespace AntonaStandard::Utilities{
    class Basic_Filter{
    protected:
        Basic_Filter* parent = nullptr;
        std::vector<Basic_Filter*> children;
    public:
        Basic_Filter(Basic_Filter* parent_){
            this->parent = parent_;
        };
        inline virtual void deposit(Basic_Filter* child){
            this->children.push_back(child);
        }
        virtual ~Basic_Filter(){
            for(auto& i:children){
                delete i;
            }
        }
    };
    template<typename type_USER,typename type_IDENTI> 
    class Filter:public Basic_Filter{
        // 所有的过滤器的内存管理都交给基类Basic_Filter处理
    protected:
        std::map<Filter*,Filter*> candidates;
    public:
        Filter(Basic_Filter* p = nullptr):Basic_Filter(p){
            // 将当前对象的内存托管给对应的父对象,父对象析构时会将其析构
            if(this->parent != nullptr){
                this->parent->deposit(this);
            }
        }        
        
        virtual void install(Filter* other);
        virtual bool remove(Filter* other);
        virtual bool dispose(type_USER* user_ptr,type_IDENTI* obj_ptr);
        virtual bool dispose(type_USER* user_ptr,type_IDENTI& obj_qut);
        
    } ;
}

namespace AntonaStandard::Utilities{
    template <typename type_USER, typename type_IDENTI>
    inline void Filter<type_USER, type_IDENTI>::install(Filter *other) {
        if(other == nullptr){
            throw AntonaStandard::Utilities::NullPointer_Error("Got a null pointer in Filter!");
        }
        this->candidates.emplace(other,other);
    }

    template <typename type_USER, typename type_IDENTI>
    inline bool Filter<type_USER, type_IDENTI>::remove(Filter *other) {
        // 从candidate中移除other但是不会删除该对象，移除成功返回true，否则false。
        auto iter = this->candidates.find(other);
        if(iter == this->candidates.end()){
            // 未找到
            return false;
        }
        // 找到
        this->candidates.erase(iter);
        return true;
    }

    template <typename type_USER, typename type_IDENTI>
    inline bool Filter<type_USER, type_IDENTI>::
    dispose(type_USER *user_ptr,type_IDENTI *obj_ptr) {
        // 默认使用候选过滤器
        bool is_dealed = false;
        for(auto& i:this->candidates){
            if(is_dealed){
                break;          // 被处理过，且不需要转发
            }
            
            is_dealed = i.second->dispose(user_ptr,obj_ptr);
        }
        return is_dealed;
    }

    template <typename type_USER, typename type_IDENTI>
    inline bool Filter<type_USER, type_IDENTI>::dispose(type_USER *user_ptr,type_IDENTI &obj_qut) {
        // 调用判别指针的版本
        return this->dispose(user_ptr,&obj_qut);
    }

}

#endif