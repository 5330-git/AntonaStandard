/**
 * @file Dll.h
 * @author Anton (yunye_helloworld@qq.com)
 * @brief 该头文件属于 CPS 组件，定义了一系列跨平台的动态库热加载函数 
 * @version 1.0.0 
 * @date 2024-03-02
 * 
 * @copyright Copyright (c) 2024
 * 
 */
#ifndef CPS_DLL_H
#define CPS_DLL_H

#include <CPS/Macros.h>
#include <Globals/Exception.h>
#include <map>
#include <string>
#include <sstream>

/**
 * @brief 导出宏，用于导出函数
 * @details
 * - **Windows 平台**
 *   - Windows 平台下函数的符号是不导出的，这意味着热加载动态库时该函数不可见
 *   - Windows 平台下该宏的展开为 `__declspec(dllexport)`
 * - **Linux 平台**
 *   - Linux 平台下的函数符号是默认导出的，但是可以通过编译器进行隐藏，这个宏可以让修饰的函数始终可见
 *   - Linux 平台下该宏的展开为  `__attribute__((visibility("default")))` 
 */
#define ASD_EXPORT ' '
#undef ASD_EXPORT     

#if AntonaStandard_PLATFORM_WINDOWS 
    #include <Windows.h>
    /// Windows平台导出宏，在想要导出的函数前使用该宏
    #define ASD_EXPORT __declspec(dllexport)    
#elif AntonaStandard_PLATFORM_LINUX
    #include <dlfcn.h>
    /// Linux平台导出宏，在编译器选项中没有指定隐藏函数的前提下，源文件下的所有函数都是默认导出的，为了防止函数默认隐藏这里设置一函数的可见性
    #define ASD_EXPORT __attribute__((visibility("default"))) 
#endif

/**
 * @brief 顶层命名空间，本项目中的所有组件都是定义在该命名空间中的
 * 
 */
namespace AntonaStandard{
    /**
     * @brief 全称为 Cross Platfrom Support，隐藏不同平台的API细节，向上提供跨平台特性的系统级API封装
     * 
     */
    namespace CPS{
        /**
         * @brief 定义跨平台的动态库句柄，用于管理加载的动态库
         * @details 
         *     在 Windows 动态库的句柄类型是 HMODULE ，而 Linux 下 动态库的句柄类型是 void*。
         * 虽然 HMODULE 在底层也是 void* 但是如果全部定义成 void* 在编译的时候可能出现报错。 
         */
        using HandlePtr_t =
        #if AntonaStandard_PLATFORM_WINDOWS 
            HMODULE;// 虽然本质都是void*指针，但是为了防止编译器报错，这里进行分别定义
        #elif AntonaStandard_PLATFORM_LINUX
            void*;
        #else
            void*; 
        #endif

        /**
         * @brief 加载动态库，重载版本一，接收原生C++字符串
         * @param path 动态库的相对路径（可执行程序）或绝对路径
         * @return HandlePtr_t 返回动态库的句柄，在访问完动态库后必须调用 unload 函数进行卸载
         *  @retval 名为 'name' 的函数的函数地址
         * @throw AntonaStandard::Globals::FailToLoadDll_Error 如果指定路径的动态库加载失败
         * @see HandlePtr_t loadDll(const std::string&)
         *      unloadDll(HandlePtr_t)
         */
        HandlePtr_t loadDll(const char* path);
        /**
         * @brief 加载动态库，重载版本二，接收std::string 对象常引用
         * @param path 动态库的相对路径（可执行程序）或绝对路径
         * @return HandlePtr_t 
         * @see HandlePtr_t loadDll(const char*)
         */
        HandlePtr_t loadDll(const std::string& path);
        /**
         * @brief 从指定的动态库中获取对应名称的函数指针
         * @param name 需要获取的函数名称
         * @param handle 函数所属的动态库的句柄
         * @return void* 
         * @throw AntonaStandard::Globals::FailToGetFunction_Error 如果指定的函数名称没有被找到
         * @see void* getFunc(const std::string& ,HandlePtr_t)
        */
        void* getFunc(const char* name,HandlePtr_t handle);
        /**
         * @brief 从指定的动态库中获取对应名称的函数指针
         * @param name 需要获取函数指针的函数名
         * @param handle 动态库的句柄 
         * @return void* 
         * @see getFunc(const char* ,HandlePtr_t)
         */
        void* getFunc(const std::string& name,HandlePtr_t handle); 
        
        /**
         * @brief 通过句柄卸载对应的动态库
         * @param handle 
         * @throw AntonaStandard::Globals::FailToUnloadDll_Error 如果动态库卸载失败会抛出该异常
         * @see loadDll(const char*)
         */
        void unloadDll(HandlePtr_t handle);
    }
}

#endif
