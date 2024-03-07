/**
 * @file Reflection.h
 * @author Anton (yunye_helloworld@qq.com)
 * @brief 实现基本的类名反射功能
 * @version 1.0.0 
 * @date 2024-03-07
 * 
 * @copyright Copyright (c) 2024
 * 
 */
#ifndef UTILITIES_REFLECTION_H
#define UTILITIES_REFLECTION_H
#include <unordered_map>
#include <memory>
#include <functional>
#include <string>
#include <Globals/Exception.h>
#include <TestingSupport/TestingMessageMacro.h>

/// @brief 变量初始化宏
#define INIT                                    \
std::function<std::shared_ptr<void>(void)> f;                   \
std::string str_name;                           \

/// @brief lambda表达式宏，用于创建类型className的构造回调函数，并通过包装器将其插入到map中

#define REGISTER(className)                     \
f =  [](){                                      \
        return std::make_shared<className>();   \
};                                              \
str_name = #className;                          \
this->func_map.insert({str_name,f});  \


/// @brief lambda表达式宏，可以为回调函数修改别名
#define REGISTER_BY_OTHERNAME(className,regist_name)       \
f =  [](){                                      \
        return std::make_shared<className>();   \
};                                              \
str_name = #regist_name;                        \
this->func_map.insert(pair<std::string,std::function<std::shared_ptr<void>(void)>>(str_name,f));  \


namespace AntonaStandard{
    namespace Utilities{
        class Reflection;
    };
};

namespace AntonaStandard::Utilities{
    // 抽象类，需要用户去派生
    /**
     * @brief 反射类
     * @details
     *      需要用户派生，在load() 函数内注册自己需要的类
     * 
     */
    class Reflection{
        TESTING_MESSAGE
    protected:
        std::unordered_map<std::string,std::function<std::shared_ptr<void>(void)>> func_map;
    public:
        Reflection(){};
        /**
         * @brief 加载函数
         * @details
         *      由用户重载，注册相关的类
         * 
         */
        virtual void load()=0;
        /**
         * @brief 根据字符串创建相关类的实例
         * 
         * @param name_str 
         * @return std::shared_ptr<void> 
         */
        virtual std::shared_ptr<void> createInstance(const char* name_str);
        /**
         * @brief 根据字符串创建相关类的实例
         * 
         * @param name_str 
         * @return std::shared_ptr<void> 
         * @see
         *      createInstance(const char*)
         */
        virtual std::shared_ptr<void> createInstance(const std::string& name_str);
    };
} // namespace AntonaStandard
#endif 
