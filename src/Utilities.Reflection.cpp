#include <Utilities/Reflection.h>
#include <sstream>
namespace AntonaStandard::Utilities{
    std::shared_ptr<void> Reflection::createInstance(const char* name_str){
             // 检查key即name_str是否存在
        if(this->func_map.count(name_str)==0){
            // 不存在抛出异常
            std::stringstream ss;
            ss<<"The type you create was not registered!: "<<name_str;
            throw AntonaStandard::Utilities::NotFound_Error(ss.str().c_str());
        }
        return this->func_map[name_str]();              // 调用对应的回调函数
    }
    
    std::shared_ptr<void> Reflection::createInstance(const std::string& name_str){
         // 检查key即name_str是否存在
        if(this->func_map.count(name_str)==0){
            // 不存在抛出异常
            std::stringstream ss;
            ss<<"The type you create was not registered!: "<<name_str;
            throw AntonaStandard::Utilities::NotFound_Error(ss.str().c_str());
        }
        return this->func_map[name_str]();              // 调用对应的回调函数
    }
}