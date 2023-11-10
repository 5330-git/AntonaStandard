#include "MultiPlatformSupport/DllSupport.h"
#include "TestingSupport/TestingMessageMacro.h"
#include <thread>
#include <chrono>
#include <memory>
// 接口类
class Interface{
    TESTING_MESSAGE
public:
    Interface()=default;
    virtual std::string getValue() = 0;
    virtual ~Interface()=default;
    void waitingPeriod(){
        std::this_thread::sleep_for(std::chrono::seconds(5));
    };
};

// 普通函数导出类接口
extern "C" {
    ASD_EXPORT std::shared_ptr<Interface> createInstance() ;
    ASD_EXPORT void destroyInstance(Interface* instance);                   // 用于测试Cross-Dll problem
}
