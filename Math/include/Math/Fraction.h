#ifndef MATH_FRACTION_H
#define MATH_FRACTION_H
/**
 * @file Fraction.h
 * @author Anton (yunye_helloworld@qq.com)
 * @brief 定义数学分数类
 * @version 1.0.0
 * @date 2024-03-07
 * 
 * @copyright Copyright (c) 2024
 * 
 */
#include <sstream>
#include <cassert>
#include <algorithm>
#include <Globals/Exception.h>
#include <TestingSupport/TestingMessageMacro.h>

namespace AntonaStandard{
    /**
     * @brief 数学组件
     * @details
     *      提供一些数学运算的工具
     */
    namespace Math{
        class Fraction;
        // 运算符，友元函数声明
        const Fraction operator+(const Fraction& lhs,const Fraction& rhs);

        const Fraction operator-(const Fraction& lhs,const Fraction& rhs);

        const Fraction operator*(const Fraction& lhs,const Fraction& rhs);

        const Fraction operator/(const Fraction& lhs,const Fraction& rhs);
            // 接收两个运算算子的引用，返回一个Fraction对象

        bool operator==(const Fraction& lhs,const Fraction& rhs);
        
        bool operator!=(const Fraction& lhs,const Fraction& rhs);

        bool operator>(const Fraction& lhs,const Fraction& rhs);
        bool operator>=(const Fraction& lhs,const Fraction& rhs);
        bool operator<(const Fraction& lhs,const Fraction& rhs);
        bool operator<=(const Fraction& lhs,const Fraction& rhs);

        std::istream& operator>>(std::istream& input,Fraction& rhs);
        std::ostream& operator<<(std::ostream& output,const Fraction& rhs);
        std::ostream& operator<<(std::ostream& output,const Fraction&& rhs);
    }
}

namespace AntonaStandard::Math{
    /**
     * @brief 分数类
     * @details
     *      支持从整数和字符串构造分数类
     */
    class Fraction{
        TESTING_MESSAGE
    private:
        int numerator;      ///< 分子
        int denominator;    ///< 分母
    public:
        /**
         * @brief 欧几里得算法，计算最大公约数，用于化简分数
         * 
         * @param num 
         * @param den 
         * @return int 
         */
        static int Euclid(int num, int den) {
            // 计算两个数的最大公因数(欧几里得算法)
            
            // 声明为静态成员函数
            int pivot;
            while (num % den != 0) {
                pivot = den;
                den = num % den;
                num = pivot;
            }
            return den;
        };
        // 保留隐式转换，减少编程工作
        Fraction(const char* str){
            std::stringstream inistream(str);
            inistream>>(*this);    
        }
        Fraction(int num=0,int den=1){
            assert(den != 0);
            int ratio = this->Euclid(num,den);
            num /=ratio;
            den /=ratio;
            if(den <0){
                num *= -1;
                den *= -1;
            }
            this->numerator = num;
            this->denominator = den;
        };
        Fraction(const Fraction& f):numerator(f.getNumerator()),denominator(f.getDenominator()){};

        Fraction& operator=(const Fraction& rhs);

        inline int getNumerator()const{
            return numerator;
        }
        // 申明为不可修改数据的函数，const常函数
        inline int getDenominator()const{
            return denominator;
        }


        // 负号运算符，将一个分数变成其相反数
        inline const Fraction  operator-(){
            return Fraction(-this->getNumerator(),this->getDenominator());
        }

        inline const Fraction operator+(){
            return *this;
        }

        inline const Fraction operator++(int){
            Fraction temp = *this;
            *this += Fraction(1);
            return temp;
        }

        inline const Fraction operator--(int){
            Fraction temp = *this;
            *this -= Fraction(1);
            return temp;
        }

        inline const Fraction operator--(){
            *this -= Fraction(1);
            return *this;
        }

        inline const Fraction operator++(){
            *this += Fraction(1);
            return *this;
        }

        Fraction& operator+=(const Fraction& f);

        Fraction& operator-=(const Fraction& f);

        Fraction& operator*=(const Fraction& f);

        Fraction& operator/=(const Fraction& f);

        friend std::istream& operator>>(std::istream& input,Fraction& f);
        
    };
}

#endif // FRACTION_H
