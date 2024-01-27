#ifndef UTILITIES_ANTONA_FOREACH_H
#define UTILITIES_ANTONA_FOREACH_H

#define Antona_foreach_AUTHOR "Anton"
#define Antona_foreach_VERSION "1.1.0"
#define Antona_foreach_EDIT_TIME "2023/1/11"
/*
*   Decoded by utf-8
*   2023/1/11 v-1.0.0 初步实现，添加了几个模板，允许使用base-range for syntax逆向迭代容器。
*   2023/8/8  v-1.1.0 修改命名空间从 AntonaStandard 到 AntonaStandard::Utilities
*/
namespace AntonaStandard{
    namespace Utilities{
        template <typename type_classTypeContainer>
        struct reverse_wrapper ;
        template <typename type_BasicTypeContainer, size_t type_size>
        struct reverse_wrapper< type_BasicTypeContainer[type_size] >;
        template <typename type_Container>
        reverse_wrapper<type_Container> r_wrap(type_Container & c);
    }
}
namespace AntonaStandard::Utilities{
    // class型容器的包装器的具体化版本
    template <typename type_classTypeContainer>
    struct reverse_wrapper {

        type_classTypeContainer & c_;
        reverse_wrapper(type_classTypeContainer & c) :  c_(c) {}

        typename type_classTypeContainer::reverse_iterator begin(){
            // base-range-for 是通过调用容器的begin函数和end函数获取迭代器工作的
            return c_.rbegin();
        }
        typename type_classTypeContainer::reverse_iterator end() {
            return c_.rend(); 
        }
    };

    template <typename type_BasicTypeContainer, size_t type_size>
    struct reverse_wrapper< type_BasicTypeContainer[type_size] >{
        // 声明目标容器的引用
        type_BasicTypeContainer (&c_)[type_size];
        reverse_wrapper( type_BasicTypeContainer(&c)[type_size] ) : c_(c) {}

        typename std::reverse_iterator<const type_BasicTypeContainer *> begin() { 
            // 使用标准库的相关函数获得反迭代器
            return std::rbegin(c_); 
        }
        typename std::reverse_iterator<const type_BasicTypeContainer *> end() { 
            return std::rend(c_); 
        }
    };

    // 方向迭代器的包装器的公共接口
    template <typename type_Container>
    reverse_wrapper<type_Container> r_wrap(type_Container & c) {
        return reverse_wrapper<type_Container>(c);
    }
}

#endif