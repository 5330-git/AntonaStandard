#include "gtest/gtest.h"
#include "Utilities/Reflection.h"
#include "TestingSupport/TestingMessageMacro.h"

class A{
    TESTING_MESSAGE
public:
    void invokeA(int& times){
        ++times;
    }
    A(){};
};
class B{
    TESTING_MESSAGE
public:
    void invokeB(int& times){
        ++times;
    }
    B(){};
};
// 设置事件监听内存释放
class ClassALeakChecker : public ::testing::EmptyTestEventListener{
public:
    long memory_usage_start_ = 0;
    long memory_usage_end_ = 0;
    ClassALeakChecker(): EmptyTestEventListener(){};
    virtual void OnTestStart(const ::testing::TestInfo& test_info) override{
        memory_usage_start_ = A::getAllocatedObjectCount();
    }
    virtual void OnTestEnd(const ::testing::TestInfo& test_info) override{
        memory_usage_end_ = A::getAllocatedObjectCount();
        EXPECT_EQ(memory_usage_start_, memory_usage_end_)<<"Memory Leak Detected! Defferences:"<<memory_usage_end_ - memory_usage_start_<<" Objcets have not been released";
    }
};

class ClassBLeakChecker : public ::testing::EmptyTestEventListener{
public:
    long memory_usage_start_ = 0;
    long memory_usage_end_ = 0;
    ClassBLeakChecker(): EmptyTestEventListener(){};
    virtual void OnTestStart(const ::testing::TestInfo& test_info) override{
        memory_usage_start_ = B::getAllocatedObjectCount();
    }
    virtual void OnTestEnd(const ::testing::TestInfo& test_info) override{
        memory_usage_end_ = B::getAllocatedObjectCount();
        EXPECT_EQ(memory_usage_start_, memory_usage_end_)<<"Memory Leak Detected! Defferences:"<<memory_usage_end_ - memory_usage_start_<<" Objcets have not been released";
    }

};

class Factory : public AntonaStandard::Utilities::Reflection{
public:
    Factory():Reflection(){this->load();};
    virtual void load()override{
        INIT;							// 初始化声明零时变量的宏
        REGISTER(A);					// 注册类A
        REGISTER(B);					// 注册类B
    }
};

TEST(Test_Reflection, Basic_usage){
    // 定义 创建一百次实例
    Factory factory;
    int invoke_A = 0;
    int invoke_B = 0;
    for(int i = 0; i < 100; i++){
        std::string class_name = "A";
        auto a = std::static_pointer_cast<A>(factory.createInstance("A"));
        a->invokeA(invoke_A);
        auto b = std::static_pointer_cast<B>(factory.createInstance("B"));
        b->invokeB(invoke_B);
        std::static_pointer_cast<A>(factory.createInstance(class_name))->invokeA(invoke_A);
    }
    EXPECT_EQ(200, invoke_A);
    EXPECT_EQ(100, invoke_B);
    // 异常创建：
    std::string class_name = "C";
    EXPECT_THROW(factory.createInstance(class_name), AntonaStandard::Utilities::NotFound_Error);
    EXPECT_THROW(factory.createInstance("C"), AntonaStandard::Utilities::NotFound_Error);

}

int main(int argc, char **argv){
    ::testing::InitGoogleTest(&argc, argv);
    // 添加内存泄漏检测器
    ::testing::TestEventListeners& listeners = ::testing::UnitTest::GetInstance()->listeners();
    listeners.Append(new ClassALeakChecker);
    listeners.Append(new ClassBLeakChecker);
    return RUN_ALL_TESTS();
}