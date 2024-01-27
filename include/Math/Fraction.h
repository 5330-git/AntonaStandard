#ifndef MATH_FRACTION_H
#define MATH_FRACTION_H

#include <sstream>
#include <cassert>
#include <algorithm>
#include <Globals/Exception.h>
#include "TestingSupport/TestingMessageMacro.h"

#define AntonaStandard_Fraction_VERSION "v-1.1.0"
#define AntonaStandard_Fraction_EDITTIME "2023/8/8"
#define AntonaStandard_Fraction_AUTHOR "Anton"

/*
*   Decoded by utf-8
*   2023/2/26 v-1.0.0 初步实现
*   2023/8/8  v-1.1.0 修改命名空间从 AntonaStandard::AntonaMath 到 AntonaStandard::Math
*/

namespace AntonaStandard{
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

    class Fraction{
        TESTING_MESSAGE
    private:
        int numerator;      // 分子
        int denominator;    // 分母
    public:
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
