#include <gtest/gtest.h>
#include <Math/Fraction.h>
#include <sstream>

// 构造方法单元测试
TEST(Test_Fraction, ConstructorTest){
    using AntonaStandard::Math::Fraction;
    using std::stringstream;

    // 从数字构造
        // 显式构造
    {
        Fraction fraction(1, 2);
        EXPECT_EQ(1, fraction.getNumerator())<<"Construct from \" Fraction fraction(1, 2);\" ";
        EXPECT_EQ(2, fraction.getDenominator())<<"Construct from \" Fraction fraction(1, 2);\" ";
    }
    
    {
        Fraction fraction(1);
        EXPECT_EQ(1, fraction.getNumerator())<<"Construct from \" Fraction fraction(1);\" ";
        EXPECT_EQ(1, fraction.getDenominator())<<"Construct from \" Fraction fraction(1);\" ";
    }
        // 隐式构造

    {
        Fraction fraction = 0;
        EXPECT_EQ(0, fraction.getNumerator())<<"Construct from \" Fraction fraction = 0;\" ";
        EXPECT_EQ(1, fraction.getDenominator())<<"Construct from \" Fraction fraction = 0;\" ";
    }

    // 从字符串构造
    {
        Fraction fraction = "1/2";
        EXPECT_EQ(1, fraction.getNumerator())<<"Construct from \" Fraction fraction = \"1/2\";\" ";
        EXPECT_EQ(2, fraction.getDenominator())<<"Construct from \" Fraction fraction = \"1/2\";\" ";
    }
    {
        Fraction fraction = "2/1";
        EXPECT_EQ(2, fraction.getNumerator())<<"Construct from \" Fraction fraction = \"2/1\";\" ";
        EXPECT_EQ(1, fraction.getDenominator())<<"Construct from \" Fraction fraction = \"2/1\";\" ";
    }
    {
        Fraction fraction = "2";
        EXPECT_EQ(2, fraction.getNumerator())<<"Construct from \" Fraction fraction = \"2\";\" ";
        EXPECT_EQ(1, fraction.getDenominator())<<"Construct from \" Fraction fraction = \"2\";\" ";
    }
    {
        Fraction fraction = "-1";
        EXPECT_EQ(-1, fraction.getNumerator())<<"Construct from \" Fraction fraction = \"-1\";\" ";
        EXPECT_EQ(1, fraction.getDenominator())<<"Construct from \" Fraction fraction = \"-1\";\" ";
    }
    {
        Fraction fraction = "-1/2";
        EXPECT_EQ(-1, fraction.getNumerator())<<"Construct from \" Fraction fraction = \"-1/2\";\" ";
        EXPECT_EQ(2, fraction.getDenominator())<<"Construct from \" Fraction fraction = \"-1/2\";\" ";
    }
    {
        Fraction fraction = "-1/-2";
        EXPECT_EQ(1, fraction.getNumerator())<<"Construct from \" Fraction fraction = \"-1/-2\";\" ";
        EXPECT_EQ(2, fraction.getDenominator())<<"Construct from \" Fraction fraction = \"-1/-2\";\" ";
    }
    {
        Fraction fraction = "1/-2";
        EXPECT_EQ(-1, fraction.getNumerator())<<"Construct from \" Fraction fraction = \"1/-2\";\" ";
        EXPECT_EQ(2, fraction.getDenominator())<<"Construct from \" Fraction fraction = \"1/-2\";\" ";
    }
    // 读取数据失败，应该设置为Fraction(0,1);
    {
        Fraction fraction = "A_B";
        EXPECT_EQ(0, fraction.getNumerator())<<"Construct from \" Fraction fraction = \"A_B\";\" ";
        EXPECT_EQ(1, fraction.getDenominator())<<"Construct from \" Fraction fraction = \"A_B\";\" ";
    }
    {
        Fraction fraction = "1/B";
        EXPECT_EQ(0, fraction.getNumerator())<<"Construct from \" Fraction fraction = \"1/B\";\" ";
        EXPECT_EQ(1, fraction.getDenominator())<<"Construct from \" Fraction fraction = \"1/B\";\" ";
    }
    {
        Fraction fraction = "1/-B";
        EXPECT_EQ(0, fraction.getNumerator())<<"Construct from \" Fraction fraction = \"1/-B\";\" ";
        EXPECT_EQ(1, fraction.getDenominator())<<"Construct from \" Fraction fraction = \"1/-B\";\" ";
    }
    {
        Fraction fraction = "-1/B";
        EXPECT_EQ(0, fraction.getNumerator())<<"Construct from \" Fraction fraction = \"-1/B\";\" ";
        EXPECT_EQ(1, fraction.getDenominator())<<"Construct from \" Fraction fraction = \"-1/B\";\" ";
    }
    {
        Fraction fraction = "A/1";
        EXPECT_EQ(0, fraction.getNumerator())<<"Construct from \" Fraction fraction = \"A/1\";\" ";
        EXPECT_EQ(1, fraction.getDenominator())<<"Construct from \" Fraction fraction = \"A/1\";\" ";
    }
    {
        Fraction fraction = "A/-1";
        EXPECT_EQ(0, fraction.getNumerator())<<"Construct from \" Fraction fraction = \"A/-1\";\" ";
        EXPECT_EQ(1, fraction.getDenominator())<<"Construct from \" Fraction fraction = \"A/-1\";\" ";
    }
    {
        Fraction fraction = "-A/1";
        EXPECT_EQ(0, fraction.getNumerator())<<"Construct from \" Fraction fraction = \"-A/1\";\" ";
        EXPECT_EQ(1, fraction.getDenominator())<<"Construct from \" Fraction fraction = \"-A/1\";\" ";
    }
    {
        Fraction fraction = "A/0";
        EXPECT_EQ(0, fraction.getNumerator())<<"Construct from \" Fraction fraction = \"A/0\";\" ";
        EXPECT_EQ(1, fraction.getDenominator())<<"Construct from \" Fraction fraction = \"A/0\";\" ";
    }
    {
        Fraction fraction = "A/B";
        EXPECT_EQ(0, fraction.getNumerator())<<"Construct from \" Fraction fraction = \"A/B\";\" ";
        EXPECT_EQ(1, fraction.getDenominator())<<"Construct from \" Fraction fraction = \"A/B\";\" ";
    }
    {
        Fraction fraction = "1/2A";
        EXPECT_EQ(1, fraction.getNumerator())<<"Construct from \" Fraction fraction = \"1/2A\";\" ";
        EXPECT_EQ(2, fraction.getDenominator())<<"Construct from \" Fraction fraction = \"1/2A\";\" ";
    }
    {
        Fraction fraction = "1/2A/3";
        EXPECT_EQ(1, fraction.getNumerator())<<"Construct from \" Fraction fraction = \"1/2A/3\";\" ";
        EXPECT_EQ(2, fraction.getDenominator())<<"Construct from \" Fraction fraction = \"1/2A/3\";\" ";
    }
    {
        Fraction fraction = "1A/2";
        EXPECT_EQ(1, fraction.getNumerator())<<"Construct from \" Fraction fraction = \"1A/2\";\" ";
        EXPECT_EQ(1, fraction.getDenominator())<<"Construct from \" Fraction fraction = \"1A/2\";\" ";
    }
    {
        Fraction fraction = "1/2//";
        EXPECT_EQ(1, fraction.getNumerator())<<"Construct from \" Fraction fraction = \"1/2//\"\"";
        EXPECT_EQ(2, fraction.getDenominator())<<"Construct from \" Fraction fraction = \"1/2//\"\"";
    }
    {
        Fraction fraction = "1//2";
        EXPECT_EQ(0, fraction.getNumerator())<<"Construct from \" Fraction fraction = \"1//2\"\"";
        EXPECT_EQ(1, fraction.getDenominator())<<"Construct from \" Fraction fraction = \"1//2\"\"";
    }
    {
        Fraction fraction = "子/母";
        EXPECT_EQ(0, fraction.getNumerator())<<"Construct from \" Fraction fraction = \"子/母\"\"";
        EXPECT_EQ(1, fraction.getDenominator())<<"Construct from \" Fraction fraction = \"子/母\"\"";
    }
    {
        Fraction fraction = "子_母";
        EXPECT_EQ(0, fraction.getNumerator())<<"Construct from \" Fraction fraction = \"子_母\"\"";
        EXPECT_EQ(1, fraction.getDenominator())<<"Construct from \" Fraction fraction = \"子_母\"\"";
    }
    

    // 拷贝构造
    {
        Fraction fraction(1,2);
        Fraction fraction1(fraction);
        EXPECT_EQ(1, fraction1.getNumerator())<<"Construct from \" Fraction fraction1(fraction);\" ";
        EXPECT_EQ(2, fraction1.getDenominator())<<"Construct from \" Fraction fraction1(fraction);\" ";
    }
    
    // 流中构造
    {
        Fraction fraction;
        std::istringstream iss("1/2");
        iss >> fraction;
        EXPECT_EQ(1, fraction.getNumerator())<<"Construct from \" std::istringstream iss(\"1/2\");\" ";
        EXPECT_EQ(2, fraction.getDenominator())<<"Construct from \" std::istringstream iss(\"1/2\");\" ";
    }

    // 异常构造, 匹配任意错误信息
    EXPECT_DEATH(Fraction(0, 0),".*");
}

// 逻辑运算测试
TEST(Test_Fraction, Operator_Logic){
    using AntonaStandard::Math::Fraction;

    Fraction fraction1(1, 2);
    Fraction fraction2(1, 3);
    // 等于
    EXPECT_FALSE(fraction1 == fraction2);
    EXPECT_TRUE(fraction1 != fraction2);
    EXPECT_TRUE(fraction1 == fraction1);
    EXPECT_FALSE(fraction1 != fraction1);
    EXPECT_FALSE(fraction1 == 1);
    EXPECT_TRUE(fraction1 != 1);
    EXPECT_TRUE(fraction1 == Fraction(1, 2));
    EXPECT_FALSE(fraction1 != Fraction(1, 2));
    EXPECT_TRUE(fraction2 == Fraction(1, 3));
    EXPECT_FALSE(fraction2 != Fraction(1, 3));
    EXPECT_TRUE(fraction1 == "1/2");
    EXPECT_FALSE(fraction1 != "1/2");
    EXPECT_TRUE(fraction1 != "1/3");

    // 大于
    EXPECT_TRUE(fraction1 > fraction2);
    EXPECT_FALSE(fraction2 > fraction1);
    // 大于等于
    EXPECT_TRUE(fraction1 >= "1/-2");
    EXPECT_TRUE(fraction1 >= "1/2");

    // 小于
    EXPECT_FALSE(fraction1 < fraction2);
    EXPECT_TRUE(fraction2 < fraction1);
    // 小于等于
    EXPECT_TRUE(fraction1 <= "1/2");
    EXPECT_FALSE(fraction1 <= "1/-2");
}


// 运算单元测试
TEST(Test_Fraction, Operator_Add) {
    using AntonaStandard::Math::Fraction;

    Fraction fraction1(1, 2);
    Fraction fraction2(1, 3);

    EXPECT_EQ("5/6",fraction1 + fraction2);
    EXPECT_EQ("5/6",fraction2 + fraction1);
    EXPECT_EQ("5/6",fraction1 + "1/3");
    EXPECT_EQ("1/6",fraction1 + "1/-3");
    EXPECT_EQ("0",fraction2 + "1/-3");
    EXPECT_EQ("1",fraction1+=fraction1);
    fraction1 = "1/2";
    EXPECT_EQ("1/2",+fraction1);
    EXPECT_EQ("-1/2",-fraction1);
    EXPECT_EQ("1/2",fraction1++);
    fraction1 = "1/2";
    EXPECT_EQ("2",fraction1 + "1/2" + 1);
    EXPECT_EQ("3/2",++fraction1);
    fraction1 = "1/2";
    EXPECT_EQ("2",1+fraction1+"1/2");
    EXPECT_EQ("2",1+Fraction("1/2")+fraction1);
    
}

TEST(Test_Fraction, Operator_Sub) {
    using AntonaStandard::Math::Fraction;

    Fraction fraction1(1, 2);
    Fraction fraction2(1, 3);

    EXPECT_EQ("1/6",fraction1 - fraction2);
    EXPECT_EQ("1/-6",fraction2 - fraction1);
    EXPECT_EQ("1/6",fraction1 - "1/3");
    EXPECT_EQ("5/6",fraction1 - "1/-3");
    EXPECT_EQ("0",fraction2 - "1/3");
    EXPECT_EQ("0",fraction1-=fraction1);
    fraction1 = "1/2";
    EXPECT_EQ("-1/2",-fraction1);
    EXPECT_EQ("1/2",fraction1--);
    fraction1 = "1/2";
    EXPECT_EQ("-1",fraction1 - "1/2" - 1);
    EXPECT_EQ("-1/2",--fraction1);
    fraction1 = "1/2";
    EXPECT_EQ("0",1-fraction1-"1/2");
    EXPECT_EQ("0",1-Fraction("1/2")-fraction1);

}


TEST(Test_Fraction, Operator_Mul) {
    using AntonaStandard::Math::Fraction;
    Fraction fraction1(1, 2);
    Fraction fraction2(1, 3);

    EXPECT_EQ("1/6",fraction1 * fraction2);
    EXPECT_EQ("1/6",fraction2 * +fraction1);
    EXPECT_EQ("1/6",+fraction2 * fraction1);
    EXPECT_EQ("1/-6",-fraction1 * fraction2);
    EXPECT_EQ("1/-6",fraction1 * -fraction2);
    // 自运算
    EXPECT_EQ("1/4",fraction1 *= fraction1);
    EXPECT_EQ("-1/16",fraction1 *= -fraction1);
    EXPECT_EQ("-1/32",fraction1 *= "1/2");
    EXPECT_EQ(-1,fraction1.getNumerator());
    EXPECT_EQ(32,fraction1.getDenominator());
}


TEST(Test_Fraction, Operator_Div) {
    using AntonaStandard::Math::Fraction;
    Fraction fraction1(1, 2);
    Fraction fraction2(1, 3);
    
    EXPECT_EQ("1/1",fraction1 / fraction1);
    EXPECT_EQ("-3/2",fraction1 / -fraction2);
    EXPECT_EQ("-3/2",-fraction1 / fraction2);
    EXPECT_EQ("3/2",+fraction1 / fraction2);
    EXPECT_EQ("3/2",fraction1 / +fraction2);

    // 自运算
    EXPECT_EQ("1/1",fraction1 /= fraction1);
    EXPECT_EQ("-3/1",fraction1 /= -fraction2);
    EXPECT_EQ("-9/1",fraction1 /= fraction2);

    // 崩溃运算
    EXPECT_DEATH(1 / Fraction(0),".*");
}

TEST(Test_Fraction, Operator_Assign){
    using AntonaStandard::Math::Fraction;
    // 赋值运算
    Fraction fraction1(1, 2);
    Fraction fraction2(1, 3);
    Fraction fraction3(1, 4);

    fraction1 = fraction2;
    EXPECT_EQ("1/3",fraction1);
    fraction1 = "1/2";
    EXPECT_EQ("1/2",fraction1);

    fraction1 = fraction2 = fraction3;
    EXPECT_EQ("1/4",fraction1);
    EXPECT_EQ("1/4",fraction2);
    EXPECT_EQ("1/4",fraction3);

    fraction1 = fraction2 = fraction3 = "1/5";
    EXPECT_EQ("1/5",fraction1);
    EXPECT_EQ("1/5",fraction2);
    EXPECT_EQ("1/5",fraction3);

    fraction1 = fraction2 = fraction3 = 6;
    EXPECT_EQ("6/1",fraction1);
    EXPECT_EQ("6/1",fraction2);
    EXPECT_EQ("6/1",fraction3);

    EXPECT_DEATH(fraction1 = 1/(fraction2 = 0),".*");
}

TEST(Test_Fraction, Operator_Stream){
    using AntonaStandard::Math::Fraction;
    std::stringstream ss;
    Fraction fraction1(1, 2);
    Fraction fraction2(1, 3);
    Fraction fraction3(1, 4);
    ss<<fraction1<<" "<<fraction2<<" "<<std::move(fraction3);
    EXPECT_EQ("1/2 1/3 1/4",ss.str());
}



