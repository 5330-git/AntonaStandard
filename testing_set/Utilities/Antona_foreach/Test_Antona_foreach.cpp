#include "gtest/gtest.h"
#include "Utilities/Antona_foreach.h"

#include <list>
#include <deque>
#include <set>
#include <string>

TEST(Test_Antona_foreach, STL_container){
    using namespace AntonaStandard::Utilities;
    // 基于标准库容器的遍历
    // 测试线性容器
    {
        std::deque<int> container1 = {1,2,3,4,5};
        std::deque<std::deque<int>> container2 = {{1,2,3},{4,5,6}};   
        auto r_iter_container1 = container1.rbegin();
        for(auto i : r_wrap(container1)){
            EXPECT_EQ(i, *r_iter_container1);
            ++r_iter_container1;
        }
        EXPECT_EQ(r_iter_container1, container1.rend());

        auto r_iter_container2 = container2.rbegin();
        for(auto i : r_wrap(container2)){
            EXPECT_EQ(i, *r_iter_container2);
            ++r_iter_container2;
        }
        EXPECT_EQ(r_iter_container2, container2.rend());
    }
    // 测试链表容器
    {
        std::list<int> container1 = {1,2,3,4,5};
        std::list<std::list<int>> container2 = {{1,2,3},{4,5,6}};
        auto r_iter_container1 = container1.rbegin();
        for(auto i : r_wrap(container1)){
            EXPECT_EQ(i, *r_iter_container1);
            ++r_iter_container1;
        }
        EXPECT_EQ(r_iter_container1, container1.rend());

        auto r_iter_container2 = container2.rbegin();
        for(auto i : r_wrap(container2)){
            EXPECT_EQ(i, *r_iter_container2);
            ++r_iter_container2;
        }
        EXPECT_EQ(r_iter_container2, container2.rend());
    }
    // 测试红黑树容器
    {
        std::set<int> container1 = {1,2,3,4,5};
        std::set<std::set<int>> container2 = {{1,2,3},{4,5,6}};
        auto r_iter_container1 = container1.rbegin();
        for(auto i : r_wrap(container1)){
            EXPECT_EQ(i, *r_iter_container1);
            ++r_iter_container1;
        }
        EXPECT_EQ(r_iter_container1, container1.rend());

        auto r_iter_container2 = container2.rbegin();
        for(auto i : r_wrap(container2)){
            EXPECT_EQ(i, *r_iter_container2);
            ++r_iter_container2;
        }
        EXPECT_EQ(r_iter_container2, container2.rend());
    }
    // 测试字符串
    {
        std::string container1 = "hello world from AntonaStadandard";
        
        auto r_iter_container1 = container1.rbegin();
        for(auto i : r_wrap(container1)){
            EXPECT_EQ(i, *r_iter_container1);
            ++r_iter_container1;
        }
        EXPECT_EQ(r_iter_container1, container1.rend());
    }
}

TEST(Test_Antona_foreach, BuildIn_container){
    using namespace AntonaStandard::Utilities;
    // 测试内建类型（数组）
    {
        int container1[] = {1, 2, 3, 4, 5};
        int container2[4][2] = {1, 2, 3, 4, 5, 6, 7, 8};
        std::vector<int> container3 = {1, 2, 3, 4, 5};
        std::vector<std::vector<int>> container4 = {{1,2},{3,4},{5,6},{7,8}};

        auto iter_container3 = container3.rbegin();
        for(auto i : r_wrap(container1)){
            EXPECT_EQ(i, *iter_container3);
            ++iter_container3;
        }
        EXPECT_EQ(iter_container3, container3.rend());

        auto iter_container4 = container4.rbegin();
        for(auto& i : r_wrap(container2)){
            auto iter_container4_inner = iter_container4->rbegin();
            for(auto j : r_wrap(i)){
                EXPECT_EQ(j, *iter_container4_inner);
                ++iter_container4_inner;
            }
            EXPECT_EQ(iter_container4_inner, iter_container4->rend());
            ++iter_container4;
        }
        EXPECT_EQ(iter_container4, container4.rend());
    }
}