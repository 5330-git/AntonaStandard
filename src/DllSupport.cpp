#include "MultiPlatformSupport/DllSupport.h"

namespace AntonaStandard::MultiPlatformSupport{
    HandlePtr_t loadDll(const char* path) { 
        HandlePtr_t handle = nullptr;

        #if AntonaStandard_PLATFORM_WINDOWS
            // Windows 平台通过LoadLibraryA 来获取动态库的句柄
        handle = LoadLibraryA(path);
        if(handle == nullptr){
            // 抛出异常：加载动态库失败
            unsigned long error_code = GetLastError();         // 获取错误代码，错误代码是双字类型定义为 unsigned long
            std::stringstream msgstream;
            msgstream<<"Fail to load dll \" "<<path<<"\",Error code:"<<error_code;
            throw Utilities::FailToLoadDll_Error(msgstream.str().c_str());
        }

        #elif AntonaStandard_PLATFORM_LINUX
            // Linux 平台通过 dlopen来获取动态库的句柄
        handle = dlopen(path,RTLD_LAZY);
        if(handle == nullptr){
            // 抛出异常：加载动态库失败
            std::stringstream msgstream;
            msgstream<<"Fail to load dll \" "<<path<<"\",Error message:"<<dlerror();
            throw Utilities::FailToLoadDll_Error(msgstream.str().c_str());
        }
        #endif
        return handle;
    }

    HandlePtr_t loadDll(const std::string& path) { 
        return loadDll(path.c_str()); 
    }

    void* getFunc(const char* name, HandlePtr_t handle) { 
        void* func = nullptr;
        #if AntonaStandard_PLATFORM_WINDOWS
            // windows平台通过GetProcAddress获取动态库中的导出函数
        func = (void*)GetProcAddress(handle,name);
        if(func == nullptr){
            // 抛出异常：获取函数地址失败
            unsigned long error_code = GetLastError();         // 获取错误代码，错误代码是双字类型定义为 unsigned long
            std::stringstream msgstream;
            msgstream<<"Fail to get Function: \" "<<name<<"\",Error code:"<<error_code;
            throw Utilities::FailToGetFunction_Error(msgstream.str().c_str());
        }
        #elif AntonaStandard_PLATFORM_LINUX
            // linux 平台通过dlsym获取动态库中的导出函数
        func = dlsym(handle,name);
        if(func == nullptr){
            // 抛出异常：获取函数地址失败
            std::stringstream msgstream;
            msgstream<<"Fail to get Function: \" "<<name<<"\",Error message:"<<dlerror();
            throw Utilities::FailToGetFunction_Error(msgstream.str().c_str());
        }
        #endif
        return func;
    }

    void* getFunc(const std::string& name, HandlePtr_t handle) {
        return getFunc(name.c_str(),handle);
    }

    void unloadDll(HandlePtr_t handle) {
        #if AntonaStandard_PLATFORM_WINDOWS
            // windows平台通过 FreeLibrary来释放动态库的句柄
        bool status = FreeLibrary(handle);              // 返回0表示失败
        if(!status){
            unsigned long error_code = GetLastError();         // 获取错误代码，错误代码是双字类型定义为 unsigned long
            std::stringstream msgstream;
            msgstream<<"Fail to unload Dll ,Error code:"<<error_code;
            throw Utilities::FailToUnloadDll_Error(msgstream.str().c_str());
        }
        #elif AntonaStandard_PLATFORM_LINUX
            // linux 平台通过dlsym获取动态库中的导出函数
        int status = dlclose(handle);                   // 返回非0表示失败
        if(status){
            std::stringstream msgstream;
            msgstream<<"Fail to unload Dll ,Error message:"<<dlerror();
            throw Utilities::FailToUnloadDll_Error(msgstream.str().c_str());
        }
        #endif
    }
}