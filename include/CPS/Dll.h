#ifndef CPS_DLL_H
#define CPS_DLL_H

#include <CPS/Macros.h>
#include <Globals/Exception.h>
#include <map>
#include <string>
#include <sstream>

#define AntonaStandard_MultiPlatformSupport_DllSupport_VERSION "1.0.0"
#define AntonaStandard_MultiPlatformSupport_DllSupport_EDIT_TIME "2023/8/17"
#define AntonaStandard_MultiPlatformSupport_DllSupport_AUTHOR "Anton"

/*
*   Decoded by UTF-8
*   2023/8/17 v-1.0.0 初步实现：提供跨平台的动态库显式链接（热加载）支持
*/

#if AntonaStandard_PLATFORM_WINDOWS 
    #include <Windows.h>
    #define ASD_EXPORT __declspec(dllexport)

#elif AntonaStandard_PLATFORM_LINUX
    #include <dlfcn.h>
    // 在编译器选项中没有指定隐藏函数的前提下，源文件下的所有函数都是默认导出的，为了防止函数默认隐藏这里设置一函数的可见性
    #define ASD_EXPORT __attribute__((visibility("default")))
#endif

// 动态库加载与卸载函数
namespace AntonaStandard{
    namespace MultiPlatformSupport{

        
        #if AntonaStandard_PLATFORM_WINDOWS 
            using HandlePtr_t = HMODULE;            // 虽然本质都是void*指针，但是为了防止编译器报错，这里进行分别定义
        #elif AntonaStandard_PLATFORM_LINUX
            using HandlePtr_t = void*;
        #endif

        HandlePtr_t loadDll(const char* path);
        HandlePtr_t loadDll(const std::string& path);

        void* getFunc(const char* name,HandlePtr_t handle);
        void* getFunc(const std::string& name,HandlePtr_t handle); 
        
        void unloadDll(HandlePtr_t handle);
    }
}

#endif