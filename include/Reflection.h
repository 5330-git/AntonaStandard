#ifndef REFLECTION_H
#define REFLECTION_H
#include <map>
#include <functional>
#include <string>
#include "Exception.h"


#define AntonaStandard_Reflection_VERSION "1.0.0"
#define AntonaStandard_Reflection_EDIT_TIME  "2023/1/2"
#define AntonaStandard_Reflection_AUTHOR "Anton"

/*
*   Decoded by utf-8
*   2023/1/2  1.1.0 - 初步实现反射机制
*/

// 变量初始化宏
#define INIT                                    \
std::function<void*(void)> f;                   \
std::string str_name;                           \

// lambda表达式宏，用于创建类型className的构造回调函数，并通过包装器将其插入到map中

#define REGISTER(className)                     \
f =  [](){                                      \
        return new className;                   \
};                                              \
str_name = #className;                          \
this->func_map.insert(pair<std::string,std::function<void*(void)>>(str_name,f));  \

namespace AntonaStandard{
    // 抽象类，需要用户去派生
    class Reflection{
    protected:
        std::map<std::string,std::function<void*(void)>> func_map;
    public:
        Reflection(){};
        virtual void load()=0;
        virtual void* createInstance(const char* name_str);
        virtual void* createInstance(const std::string& name_str);
    };
} // namespace AntonaStandard
#endif 