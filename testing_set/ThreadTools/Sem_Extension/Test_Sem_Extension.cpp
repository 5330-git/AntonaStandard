#include <gtest/gtest.h>
#include <semaphore>
#include <chrono>
#include <functional>
#include <thread>
#include <ThreadTools/Sem_Extension.h>

void semaphore_release(std::counting_semaphore<>& sem,int num){
    // 等待5秒后释放
    std::this_thread::sleep_for(std::chrono::milliseconds(3000));
    sem.release(num);
}

TEST(Test_Sem_Extension, And_Aquirer){
    using namespace AntonaStandard::ThreadTools;
    // 检查都有信号量可取时才可以锁定
    std::counting_semaphore c_sem1(1);
    std::counting_semaphore c_sem2(1);
    std::counting_semaphore c_sem3(1);
    std::counting_semaphore c_sem4(0);
    std::counting_semaphore c_sem5(0);
    std::binary_semaphore b_sem1(true);
    And_Sem_Acquirer ac;
    // 都有信号量，可以申请成功
    ac.and_acquire(c_sem1, c_sem2, c_sem3);
    EXPECT_FALSE(c_sem1.try_acquire());
    
    std::thread t1(&semaphore_release, std::ref(c_sem1),1);
    std::thread t2(&semaphore_release, std::ref(c_sem2),1);
    std::thread t3(&semaphore_release, std::ref(c_sem3),1);
    std::thread t4(&semaphore_release, std::ref(c_sem4),1);
    std::thread t5(&semaphore_release, std::ref(c_sem5),1);

    // 只有c_sem5 有剩余，必须等待线程释放信号量
    ac.and_acquire(c_sem1, c_sem2, c_sem3, c_sem4, c_sem5, b_sem1);
    EXPECT_FALSE(c_sem1.try_acquire());
    t1.join();
    t2.join();
    t3.join();
    t4.join();
    t5.join();
    // 释放信号量，保证所有信号量都有一个可用
    ac.and_release(c_sem1, c_sem2, c_sem3, c_sem4, c_sem5, b_sem1);
    ac.and_acquire(c_sem1, c_sem2, c_sem3, c_sem4,b_sem1);
    EXPECT_FALSE(c_sem1.try_acquire());
    EXPECT_FALSE(c_sem2.try_acquire());
    EXPECT_FALSE(c_sem3.try_acquire());
    EXPECT_FALSE(c_sem4.try_acquire());
    EXPECT_TRUE(c_sem5.try_acquire());
    EXPECT_FALSE(b_sem1.try_acquire());
}

TEST(Test_Sem_Extension, Sem_set_Acquirer){
    using AntonaStandard::ThreadTools::Sem_Set_Acquirer;
    Sem_Set_Acquirer ssac;
    std::counting_semaphore c_sem1(1);
    std::counting_semaphore c_sem2(1);
    std::counting_semaphore c_sem3(1);
    std::counting_semaphore c_sem4(0);
    std::counting_semaphore c_sem5(0);
    std::binary_semaphore b_sem1(true);

    ssac.sem_set_acquire(c_sem1,1,0, c_sem2,1,0, c_sem3,1,0);
    EXPECT_TRUE(c_sem1.try_acquire());
    EXPECT_TRUE(c_sem2.try_acquire());
    EXPECT_TRUE(c_sem3.try_acquire());

    ssac.sem_set_release(c_sem1,1,c_sem2,1,c_sem3,2,c_sem4,2,c_sem5,3);
    std::thread t1(&semaphore_release, std::ref(c_sem1),5);
    std::thread t2(&semaphore_release, std::ref(c_sem2),5);
    std::thread t3(&semaphore_release, std::ref(c_sem3),4);
    std::thread t4(&semaphore_release, std::ref(c_sem4),4);
    std::thread t5(&semaphore_release, std::ref(c_sem5),3);
    ssac.sem_set_acquire(c_sem1,6,6,c_sem2,3,6,c_sem3,3,6,c_sem4,4,6,c_sem5,5,6,b_sem1,0,1);
    EXPECT_FALSE(c_sem1.try_acquire());
    EXPECT_FALSE(c_sem2.try_acquire());
    EXPECT_FALSE(c_sem3.try_acquire());
    EXPECT_FALSE(c_sem4.try_acquire());
    EXPECT_FALSE(c_sem5.try_acquire());

    // 测试特化版本
    std::thread t6(&semaphore_release, std::ref(c_sem1),5);
    ssac.sem_set_acquire(c_sem1,5,5);
    EXPECT_FALSE(c_sem1.try_acquire());

    t1.join();
    t2.join();
    t3.join();
    t4.join();
    t5.join();
    t6.join();
}