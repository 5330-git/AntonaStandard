/**
 * @file Antona_foreach.h
 * @author Anton (yunye_helloworld@qq.com)
 * @brief 实现base on range 基于范围的for循环的逆向迭代包装器
 * @details
 *      - 如下是一段 base on range for 循环的应用
 * @code
 *      int a[] = {1,2,3,4};
 *      std::vector<int> b = {1,2,3,4};
 *      for(auto i : a){
 *          std::cout<<i<<" ";
 *      }
 *      std::cout<<endl;
 *      for(auto i : b){
 *          std::cout<<i<<" ";
 *      }
 * // 这两个循环都可以都打印 "1 2 3 4 "
 * @endcode 
 *      其本质是调用 begin() 函数和 end() 函数分别获得首迭代器和尾迭代器实现的（对于数组它会调用std::begin 和 std::end包装出这样一个迭代器）
 *      而 rbegin() 和 rend() 可以获取反向迭代器，我们只需要包装一个类，但是 base on range for 不会调用 rbegin 和 rend
 *      
 *      - 解决办法也比较简单：我们构造一个封装类，其中有两个接口 begin() 和 end() 而实际上我们在里面调用的是 rbegin()
 *      和 rend() 这样就将反向迭代器伪装成了一个正向迭代器
 * @version 0.1
 * @date 2024-03-07
 * 
 * @copyright Copyright (c) 2024
 * 
 */
#ifndef UTILITIES_ANTONA_FOREACH_H
#define UTILITIES_ANTONA_FOREACH_H

namespace AntonaStandard{
    /**
     * @brief 实用功能（杂项）组件
     * 
     */
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
    /**
     * @brief 包装迭代器接口，用于class型容器
     * 
     * @tparam type_classTypeContainer 
     */
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

    /**
     * @brief 包装迭代器接口，用于数组
     * 
     * @tparam type_BasicTypeContainer 
     * @tparam type_size 
     */
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
    /**
     * @brief 获取容器或数组的包装器的接口
     * 
     * @tparam type_Container 
     * @param c 
     * @return reverse_wrapper<type_Container> 
     */
    template <typename type_Container>
    reverse_wrapper<type_Container> r_wrap(type_Container & c) {
        return reverse_wrapper<type_Container>(c);
    }
}

#endif