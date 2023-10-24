#ifndef FRACTION_H
#define FRACTION_H

#include <sstream>
#include "Utilities/Exception.h"

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
    private:
        int numerator;      // 分子
        int denominator;    // 分母
    public:
        static int Euclid(int lhs, int rhs) {
            // 计算两个数的最大公因数(欧几里得算法)
            // 声明为静态成员函数
            int pivot;
            while (lhs % rhs != 0) {
                pivot = rhs;
                rhs = lhs % rhs;
                lhs = pivot;
            }
            return rhs;
        };
        // 保留隐式转换，减少编程工作
        Fraction(const char* str){
            this->denominator = 1;
            std::stringstream inistream;
            inistream<<str;
            inistream>>(*this);    
        }
        Fraction(int num=0,int den=1):numerator(num),denominator(den){};
        Fraction(const Fraction& f):numerator(f.getNumerator()),denominator(f.getDenominator()){};

        Fraction& operator=(const Fraction& rhs);

        inline int getNumerator()const{
            return numerator;
        }
        // 申明为不可修改数据的函数，const常函数
        inline int getDenominator()const{
            return denominator;
        }

        inline const Fraction inverse(){
            //返回这个分数的倒数
            return Fraction(this->getDenominator(),this->getNumerator());
        }

        // 负号运算符，将一个分数变成其相反数
        inline const Fraction  operator-(){
            return Fraction(-this->numerator,this->denominator);
        }
        Fraction& operator+=(const Fraction& f);

        Fraction& operator-=(const Fraction& f);

        Fraction& operator*=(const Fraction& f);

        Fraction& operator/=(const Fraction& f);

        friend std::istream& operator>>(std::istream& input,Fraction& f);
        
    };
}

#endif // FRACTION_H
