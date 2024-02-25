#include "gtest/gtest.h"
#include "Utilities/Delegate.h"
#include <vector>
#include <iostream>
// 派生实现事件监听者，用于监听new 与 delete的情况，来判断是否发生内存泄漏

// 需要测试的Delegate 类型如下

using namespace AntonaStandard::Utilities;
class NonReturnBaseFuncPointerContainerLeakChecker : public ::testing::EmptyTestEventListener{
public:
// 
    long memory_usage_start_=0;
    long memory_usage_end_=0;
    // 只测试有返回值和无返回值的两类委托
    NonReturnBaseFuncPointerContainerLeakChecker() : EmptyTestEventListener(){}
    virtual void OnTestStart(const ::testing::TestInfo& test_info) override{
        // 开始测试
        memory_usage_start_ = AntonaStandard::Utilities::BaseFuncPointerContainer<void>::getAllocatedObjectCount();
    }
    virtual void OnTestEnd(const ::testing::TestInfo& test_info) override{
        // 结束测试
        // 记录当前内存使用情况
        memory_usage_end_ = AntonaStandard::Utilities::BaseFuncPointerContainer<void>::getAllocatedObjectCount();

        EXPECT_EQ(memory_usage_start_, memory_usage_end_)<<"Memory Leak Detected! Defferences:"<<memory_usage_end_ - memory_usage_start_<<" Objcets have not been released";
    }
    
};

class ReturnBaseFuncPointerContainerLeakChecker : public ::testing::EmptyTestEventListener{
public:
    long memory_usage_start_=0;
    long memory_usage_end_=0;
    // 只测试有返回值和无返回值的两类委托
    ReturnBaseFuncPointerContainerLeakChecker() : EmptyTestEventListener(){}
    virtual void OnTestStart(const ::testing::TestInfo& test_info) override{
        // 开始测试
        memory_usage_start_ = AntonaStandard::Utilities::BaseFuncPointerContainer<int>::getAllocatedObjectCount();
        
    }
    virtual void OnTestEnd(const ::testing::TestInfo& test_info) override{
        // 结束测试
        // 记录当前内存使用情况
        memory_usage_end_ = AntonaStandard::Utilities::BaseFuncPointerContainer<int>::getAllocatedObjectCount();
        
        EXPECT_EQ(memory_usage_start_, memory_usage_end_)<<"Memory Leak Detected! Defferences:"<<memory_usage_end_ - memory_usage_start_<<" Objcets have not been released";
    }
    
};


// 定义委托函数
int invoke_times = 0;
class A{
public:
    static int s_i_v_A(){
        ++invoke_times;
        return 10;
    }
    int i_v_A(){
        ++invoke_times;
        return 11;
    }

};
int i_v(){
    ++invoke_times;
    return 12;
}
class B{
public:
    static void s_v_v_B(){
        ++invoke_times;
    }
    
    void v_v_B(){
        ++invoke_times;
    }
};
void v_v(){
    ++invoke_times;
}

class C{
public:
    static int& s_iq_iq_C(int& v){
        ++invoke_times;
        return v;
    }
    int& iq_iq_C(int& v){
        ++invoke_times;
        return v;
    }
};
int& iq_iq(int& v){
    ++invoke_times;
    return v;
}

// 定义测试夹具
class DelegateTestFixture : public ::testing::Test{
public:
    A a;
    B b;
    C c;

    // 实例化委托类
    Delegate<int(void)> del;
    Delegate<void()> del1;
    Delegate<int&(int&)> del2;
    DelegateTestFixture() : Test(){}
    virtual void SetUp() override{
        // 测试前的数据准备
        invoke_times = 0;
        // 添加委托
        del += newDelegate(A::s_i_v_A);
        del += newDelegate(a,&A::i_v_A);         // 对于写法： del += newDelegate(a,a.i_v_A); 在Linux环境下会报错，这里同一写成现在的样子
        del += newDelegate(i_v);

        del1 += newDelegate(B::s_v_v_B);
        del1 += newDelegate(b,&B::v_v_B);
        del1 += newDelegate(v_v);

        del2 += newDelegate(C::s_iq_iq_C);
        del2 += newDelegate(c,&C::iq_iq_C);
        del2 += newDelegate(iq_iq);

        EXPECT_EQ(del.size(), 3);
        EXPECT_EQ(del1.size(), 3);
        EXPECT_EQ(del2.size(), 3);
        
    }
};


TEST_F(DelegateTestFixture, Add_Delegate){
    // 重复的委托不会被添加
    del += newDelegate(A::s_i_v_A);
    del += newDelegate(a,&A::i_v_A);         // 对于写法： del += newDelegate(a,a.i_v_A); 在Linux环境下会报错，这里同一写成现在的样子
    del += newDelegate(i_v);

    del1 += newDelegate(B::s_v_v_B);
    del1 += newDelegate(b,&B::v_v_B);
    del1 += newDelegate(v_v);

    del2 += newDelegate(C::s_iq_iq_C);
    del2 += newDelegate(c,&C::iq_iq_C);
    del2 += newDelegate(iq_iq);
    
    EXPECT_EQ(del.size(), 3);
    EXPECT_EQ(del1.size(), 3);
    EXPECT_EQ(del2.size(), 3);
}

TEST_F(DelegateTestFixture, Remove_Delegate){
    // 乱序删除委托
    del -= newDelegate(a,&A::i_v_A);         // 对于写法： del += newDelegate(a,a.i_v_A); 在Linux环境下会报错，这里同一写成现在的样子
    del -= newDelegate(A::s_i_v_A);
    del -= newDelegate(i_v);

    
    del1 -= newDelegate(v_v);
    del1 -= newDelegate(B::s_v_v_B);
    del1 -= newDelegate(b,&B::v_v_B);

    
    del2 -= newDelegate(c,&C::iq_iq_C);
    del2 -= newDelegate(iq_iq);
    del2 -= newDelegate(C::s_iq_iq_C);

    EXPECT_TRUE(del.empty());
    EXPECT_TRUE(del1.empty());
    EXPECT_TRUE(del2.empty());

    // 溢出不存在的委托会触发异常
    EXPECT_THROW(del -= newDelegate(A::s_i_v_A), std::exception);
    EXPECT_THROW(del -= newDelegate(a,&A::i_v_A), std::exception);
    EXPECT_THROW(del -= newDelegate(i_v), std::exception);

    EXPECT_THROW(del1 -= newDelegate(v_v), std::exception);
    EXPECT_THROW(del1 -= newDelegate(B::s_v_v_B), std::exception);
    EXPECT_THROW(del1 -= newDelegate(b,&B::v_v_B), std::exception);

    EXPECT_THROW(del2 -= newDelegate(c,&C::iq_iq_C), std::exception);
    EXPECT_THROW(del2 -= newDelegate(iq_iq), std::exception);
    EXPECT_THROW(del2 -= newDelegate(C::s_iq_iq_C), std::exception);


}

TEST_F(DelegateTestFixture, Invoke_Delegate){
    std::vector<int> vec = del();			// 返回值为int，无参数列表调用演示
    
    del.call_without_return();			// 无返回值调用办法
    del1();								// 无返回值，无参数调用演示
    int x = 666;
    std::vector<int> vec2 = del2(x);			// 返回值为int,int&参数调用演示
    EXPECT_EQ(vec.size(), del2.size());
    EXPECT_EQ(del.size()*2 + del1.size() + del2.size(), invoke_times);
}

// 测试委托的equal接口
TEST_F(DelegateTestFixture, Equal_Delegate){
    auto d1 = newDelegate(A::s_i_v_A);
    auto d11 = newDelegate(a,&A::i_v_A);

    auto d2 = newDelegate(b,&B::v_v_B);
    auto d22 = newDelegate(v_v);

    auto d3 = newDelegate(iq_iq);
    auto d33 = newDelegate(c,&C::iq_iq_C);

    EXPECT_TRUE(d1->equal(d1));
    EXPECT_TRUE(d2->equal(d2));
    EXPECT_TRUE(d3->equal(d3));

    EXPECT_FALSE(d1->equal(d11));
    EXPECT_FALSE(d2->equal(d22));
    EXPECT_FALSE(d3->equal(d33));

    EXPECT_FALSE(d1->equal(nullptr));
    EXPECT_FALSE(d2->equal(nullptr));
    EXPECT_FALSE(d3->equal(nullptr));

    // 删除委托
    delete d1;
    delete d2;
    delete d3;
    delete d11;
    delete d22;
    delete d33;
}

// 测试委托的clone接口 (浅拷贝接口)
TEST_F(DelegateTestFixture, Clone_Delegate){
    auto d1 = newDelegate(A::s_i_v_A);
    auto d2 = newDelegate(b,&B::v_v_B);
    auto d3 = newDelegate(iq_iq);

    EXPECT_EQ(d1->clone(), d1);
    EXPECT_EQ(d2->clone(), d2);
    EXPECT_EQ(d3->clone(), d3);

    delete d1;
    delete d2;
    delete d3;

}

TEST_F(DelegateTestFixture, Copy_Container){
    auto d1 = newDelegate(A::s_i_v_A);
    auto d2 = newDelegate(b,&B::v_v_B);
    auto d3 = newDelegate(iq_iq);
    auto d4 = newDelegate(i_v);
    auto d5 = newDelegate(v_v);

    auto d1c = d1->copy();
    auto d2c = d2->copy();
    auto d3c = d3->copy();
    auto d4c = d4->copy();
    auto d5c = d5->copy();

    EXPECT_FALSE(d1==d1c);
    EXPECT_FALSE(d2==d2c);
    EXPECT_FALSE(d3==d3c);

    delete d1;
    delete d2;
    delete d3;
    delete d4;
    delete d5;
    delete d1c;
    delete d2c;
    delete d3c;
    delete d4c;
    delete d5c;
}
int main(int argc, char **argv){
    ::testing::InitGoogleTest(&argc, argv);
    // 添加内存泄漏检测器
    ::testing::TestEventListeners& listeners = ::testing::UnitTest::GetInstance()->listeners();
    listeners.Append(new NonReturnBaseFuncPointerContainerLeakChecker);
    listeners.Append(new ReturnBaseFuncPointerContainerLeakChecker);
    
    return RUN_ALL_TESTS();
}

