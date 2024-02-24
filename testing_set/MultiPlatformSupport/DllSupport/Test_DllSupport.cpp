#include "Interfaces.h"
#include "MultiPlatformSupport/DllSupport.h"
#include "MultiPlatformSupport/MultiPlatformMacro.h"
#include "gtest/gtest.h"
#include <functional>
#include <chrono>
#include <iostream>
#include <thread>

std::string ROOT_PATH = TEST_PROJECT_DIR ;

// 设置 Interface 的监听器，监听其是否内存泄漏
class InterfaceLeakChecker: public ::testing::EmptyTestEventListener{
public:
    long memory_usage_start_ = 0;
    long memory_usage_end_ = 0;
    virtual void OnTestStart(const ::testing::TestInfo& test_info){
        memory_usage_start_ = Interface::getAllocatedObjectCount();   
    }
    virtual void OnTestEnd(const ::testing::TestInfo& test_info){
        memory_usage_end_ = Interface::getAllocatedObjectCount();
        EXPECT_EQ(memory_usage_start_, memory_usage_end_)<<"Memory Leak Detected! Defferences:"<<memory_usage_end_ - memory_usage_start_<<" Objcets have not been released";
    }
};

TEST(Test_DllSupport, Normal_usage){
    using namespace AntonaStandard::MultiPlatformSupport;
    // 正常使用的测试
    // 加载ImplementA 和 ImplementB动态库
    #if AntonaStandard_PLATFORM_LINUX
        std::string dllImplementA = "libImplementA.so";
        const char* dllImplementB = "libImplementB.so";
    #elif AntonaStandard_PLATFORM_WINDOWS
        std::string dllImplementA = "libImplementA.dll";
        const char* dllImplementB = "libImplementB.dll";
    #endif

    HandlePtr_t dllImplementA_handle = loadDll(ROOT_PATH + dllImplementA);
    HandlePtr_t dllImplementB_handle = loadDll(ROOT_PATH + dllImplementB);

    // 获取接口函数
    using Interface_func_t = decltype(&createInstance);
    Interface_func_t interfaceA = (Interface_func_t)(getFunc("createInstance",dllImplementA_handle));
    Interface_func_t interfaceB = (Interface_func_t)(getFunc(std::string("createInstance"),dllImplementB_handle));
    // 创建100 个对象并调用接口
    for(int i = 0;i<100;++i){
        auto objA = interfaceA();
        auto objB = interfaceB();
        EXPECT_EQ(objA->getValue(),"A");
        EXPECT_EQ(objB->getValue(),"B");
    }
    // 销毁句柄
    unloadDll(dllImplementA_handle);
    unloadDll(dllImplementB_handle);
}

TEST(Test_DllSupport, Test_CrossDllProblem){
    using namespace AntonaStandard::MultiPlatformSupport;
    // 加载ImplementA 和 ImplementB动态库
    #if AntonaStandard_PLATFORM_LINUX
        std::string dllImplementA = "libImplementA.so";
        const char* dllImplementB = "libImplementB.so";
    #elif AntonaStandard_PLATFORM_WINDOWS
        std::string dllImplementA = "libImplementA.dll";
        const char* dllImplementB = "libImplementB.dll";
    #endif

    HandlePtr_t dllImplementA_handle = loadDll(ROOT_PATH + dllImplementA);
    HandlePtr_t dllImplementB_handle = loadDll(ROOT_PATH + dllImplementB);

    // 获取接口函数
    using Interface_func_t = decltype(&createInstance);
    using Destory_func_t = decltype(&destroyInstance);

    Interface_func_t interfaceA = (Interface_func_t)(getFunc("createInstance",dllImplementA_handle));
    Interface_func_t interfaceB = (Interface_func_t)(getFunc(std::string("createInstance"),dllImplementB_handle));
    auto interfaceA_destroy = (Destory_func_t)(getFunc("destroyInstance",dllImplementA_handle));
    auto interfaceB_destroy = (Destory_func_t)(getFunc(std::string("destroyInstance"),dllImplementB_handle));
    // 创建一个A对象
    auto objA = interfaceA();
    auto objB = interfaceB();
    auto ptrA = objA.get();
    auto ptrB = objB.get();
    // 重置智能指针，使用对应的destroyInstance 销毁对象
    objA.reset();
    objB.reset();
    EXPECT_DEATH(interfaceB_destroy(ptrA),".*");    
    // interfaceB_destroy(ptrA);                // 可能出现段错误

    EXPECT_DEATH(interfaceA_destroy(ptrB),".*");

    EXPECT_DEATH({
        // 智能指针 objC 和 objD是在动态库中创建的，而此时动态库已经被释放，其对应的析构函数地址已经无效
        // 产生了非常隐蔽的 cross-Dll 问题
        auto objC = interfaceB();
        auto objD = interfaceA();
        // 销毁句柄
        unloadDll(dllImplementA_handle);
        unloadDll(dllImplementB_handle);
    },".*");
    // 销毁句柄
    unloadDll(dllImplementA_handle);
    unloadDll(dllImplementB_handle);
    
}

// 测试加载错误

TEST(Test_DllSupport, Fail_To_load){
    using namespace AntonaStandard::MultiPlatformSupport;
    // 加载ImplementA 和 ImplementB动态库(错误的路径或名称)
    #if AntonaStandard_PLATFORM_LINUX
        std::string dllImplementA = "libImplementAA.so";
        const char* dllImplementB = "libImplementBB.so";
    #elif AntonaStandard_PLATFORM_WINDOWS
        std::string dllImplementA = "libImplementAA.dll";
        const char* dllImplementB = "libImplementBB.dll";
    #endif
    EXPECT_THROW(loadDll(ROOT_PATH + dllImplementA),AntonaStandard::Utilities::FailToLoadDll_Error);
    EXPECT_THROW(loadDll(ROOT_PATH + dllImplementB),AntonaStandard::Utilities::FailToLoadDll_Error);
}

// 测试获取函数错误
TEST(Test_DllSupport, Fail_To_GetFunc){
    using namespace AntonaStandard::MultiPlatformSupport;
    
    // 加载ImplementA 和 ImplementB动态库
    #if AntonaStandard_PLATFORM_LINUX
        std::string dllImplementA = "libImplementA.so";
        const char* dllImplementB = "libImplementB.so";
    #elif AntonaStandard_PLATFORM_WINDOWS
        std::string dllImplementA = "libImplementA.dll";
        const char* dllImplementB = "libImplementB.dll";
    #endif

    HandlePtr_t dllImplementA_handle = loadDll(ROOT_PATH + dllImplementA);
    HandlePtr_t dllImplementB_handle = loadDll(ROOT_PATH + dllImplementB);

    // 获取接口函数
    using Interface_func_t = decltype(&createInstance); 
    EXPECT_THROW((Interface_func_t)(getFunc("create",dllImplementA_handle)),AntonaStandard::Utilities::FailToGetFunction_Error);
    EXPECT_THROW((Interface_func_t)(getFunc(std::string("create"),dllImplementB_handle)),AntonaStandard::Utilities::FailToGetFunction_Error);
    Interface_func_t interfaceA = (Interface_func_t)(getFunc("createInstance",dllImplementA_handle));
    Interface_func_t interfaceB = (Interface_func_t)(getFunc(std::string("createInstance"),dllImplementB_handle));
    
    unloadDll(dllImplementA_handle);
    unloadDll(dllImplementB_handle);
    
        // 库释放后再调用getFunc会导致异常
    EXPECT_THROW({
        // 动态库释放需要一定时间，这里添加一个循环，使得可以触发崩溃
        for(int i = 0; i < 10000; i++){
            Interface_func_t interfaceAA = (Interface_func_t)(getFunc("createInstance",dllImplementA_handle));
            Interface_func_t interfaceBB = (Interface_func_t)(getFunc(std::string("createInstance"),dllImplementB_handle));
        }
    },AntonaStandard::Utilities::FailToGetFunction_Error);
}

TEST(Test_DllSupport,Fail_to_invoke){
    using namespace AntonaStandard::MultiPlatformSupport;
    
    // 加载ImplementA 和 ImplementB动态库
    #if AntonaStandard_PLATFORM_LINUX
        std::string dllImplementA = "libImplementA.so";
        const char* dllImplementB = "libImplementB.so";
    #elif AntonaStandard_PLATFORM_WINDOWS
        std::string dllImplementA = "libImplementA.dll";
        const char* dllImplementB = "libImplementB.dll";
    #endif

    HandlePtr_t dllImplementA_handle = loadDll(ROOT_PATH + dllImplementA);
    HandlePtr_t dllImplementB_handle = loadDll(ROOT_PATH + dllImplementB);

    // 获取接口函数
    using Interface_func_t = decltype(&createInstance); 
    Interface_func_t interfaceA = (Interface_func_t)(getFunc("createInstance",dllImplementA_handle));
    Interface_func_t interfaceB = (Interface_func_t)(getFunc(std::string("createInstance"),dllImplementB_handle));

    unloadDll(dllImplementA_handle);
    unloadDll(dllImplementB_handle);

    // 库释放后再调用内部的函数会导致段错误（函数地址失效）
     EXPECT_DEATH({
        // 动态库释放需要一定时间，这里添加一个循环，使得可以触发崩溃
        for(int i = 0; i < 10000; i++){
            auto interfaceA_result = interfaceA();
            auto interfaceB_result = interfaceB();
        }
    },".*");
}

// 测试重复销毁句柄导致的卸载失败
TEST(Test_DllSupport,Fail_to_Unload){
    using namespace AntonaStandard::MultiPlatformSupport;
    // 加载ImplementA 和 ImplementB动态库
    #if AntonaStandard_PLATFORM_LINUX
        std::string dllImplementA = "libImplementA.so";
        const char* dllImplementB = "libImplementB.so";
    #elif AntonaStandard_PLATFORM_WINDOWS
        std::string dllImplementA = "libImplementA.dll";
        const char* dllImplementB = "libImplementB.dll";
    #endif
    HandlePtr_t dllImplementA_handle = loadDll(ROOT_PATH + dllImplementA);
    HandlePtr_t dllImplementB_handle = loadDll(ROOT_PATH + dllImplementB);

    using Interface_func_t = decltype(&createInstance); 
    Interface_func_t createInstance_func = (Interface_func_t)(getFunc("createInstance",dllImplementA_handle));
    Interface_func_t createInstance_func2 = (Interface_func_t)(getFunc("createInstance",dllImplementB_handle));

    // 销毁句柄
    unloadDll(dllImplementA_handle);
    unloadDll(dllImplementB_handle);

    EXPECT_THROW(
        {
            // 动态库的释放比较耗时，所以添加循环，使得可以触发异常
            for(int i = 0; i < 10000; ++i){
                unloadDll(dllImplementA_handle);
                unloadDll(dllImplementB_handle);
            }   
        }
        ,AntonaStandard::Utilities::FailToUnloadDll_Error);
}




int main(int argc, char **argv){
    ::testing::InitGoogleTest(&argc, argv);
    
    // 添加内存泄漏检测器
    ::testing::TestEventListeners& listeners = ::testing::UnitTest::GetInstance()->listeners();
    listeners.Append(new InterfaceLeakChecker);
    
    return RUN_ALL_TESTS();
}