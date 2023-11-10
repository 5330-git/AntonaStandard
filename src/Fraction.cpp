#include "Math/Fraction.h"
namespace AntonaStandard::Math{
        /*			"+",加法重载部分					*/
    // 友元函数是可以通过成员访问运算符访问私有成员的
    const Fraction operator+(const Fraction& lhs, const Fraction& rhs) {
        //	如果声明中加了const，定义中没有加const，可能会出现无权访问私有成员的报错
        // 加法友元重载函数定义
        int retnum = lhs.getNumerator() * rhs.getDenominator() + rhs.getNumerator() * lhs.getDenominator();
        int retden = lhs.getDenominator() * rhs.getDenominator();
        return 	Fraction(retnum , retden);	// 在使用的时候编译器会自动调用构造函数Fraction(const Fraction&),用来处理作用域问题（深复制了）
    }

    /*			"-",减法重载部分					*/
    const Fraction operator-(const Fraction& lhs, const Fraction& rhs) {
        // 减法友元重载函数定义
        int retnum = lhs.getNumerator() * rhs.getDenominator() - rhs.getNumerator() * lhs.getDenominator();
        int retden = lhs.getDenominator() * rhs.getDenominator();
        return 	Fraction(retnum , retden );	// 在使用的时候编译器会自动调用Fraction默认构造函数,用来处理作用域问题（深复制了）
    }

    /*			"*",乘法重载部分					*/
    const Fraction operator*(const Fraction& lhs, const Fraction& rhs) {
        // 加法友元重载函数定义
        int retnum = lhs.getNumerator() * rhs.getNumerator();
        int retden = lhs.getDenominator() * rhs.getDenominator();
        return 	Fraction(retnum , retden);
    }

    /*          '/'乘法重载                 */
    const Fraction operator/(const Fraction& lhs, const Fraction& rhs) {
        // 减法友元重载函数定义
        int retnum = lhs.getNumerator() * rhs.getDenominator();
        int retden = lhs.getDenominator() * rhs.getNumerator();
        return 	Fraction(retnum, retden);
    }

    // "+="运算符作为成员函数进行重载
    Fraction& Fraction::operator+=(const Fraction& f) {
        int retnum = this->numerator * f.getDenominator() + this->denominator * f.getNumerator();
        int retden = this->denominator * f.getDenominator();
        (*this) = Fraction(retnum, retden);
        return (*this);
    }

    // "-="运算符作为成员函数进行重载

    Fraction& Fraction::operator-=(const Fraction& f) {
        int retnum = this->numerator * f.getDenominator() - this->denominator * f.getNumerator();
        int retden = this->denominator * f.getDenominator();
        (*this) = Fraction(retnum, retden);
        return (*this);
    }

    // "*="运算符作为成员函数进行重载
    Fraction& Fraction::operator*=(const Fraction& f) {
        int retnum = this->numerator * f.getNumerator();
        int retden = this->denominator * f.getDenominator();
        (*this) = Fraction(retnum, retden);
        return *this;

    }
    Fraction& Fraction::operator/=(const Fraction& f) {
        int retnum = this->numerator * f.getDenominator();
        int retden = this->denominator * f.getNumerator();
        (*this) = Fraction(retnum, retden);
        return *this;

    }
    Fraction& Fraction::operator=(const Fraction& rhs){
        this->numerator = rhs.getNumerator();
        this->denominator = rhs.getDenominator();
        return *this;
    }
    
    bool operator==(const Fraction& lhs, const Fraction& rhs){
        int lhs_num = lhs.getNumerator();
        int lhs_den = lhs.getDenominator();
        int rhs_num = rhs.getNumerator();
        int rhs_den = rhs.getDenominator();
        int res_num = lhs_num*rhs_den;
        int res_den = lhs_den*rhs_num;
        bool result = false;
        if(res_num == res_den){
            result = true;
        }
        return result;
    }


    bool operator!=(const Fraction& lhs, const Fraction& rhs){
        int lhs_num = lhs.getNumerator();
        int lhs_den = lhs.getDenominator();
        int rhs_num = rhs.getNumerator();
        int rhs_den = rhs.getDenominator();
        int res_num = lhs_num*rhs_den;
        int res_den = lhs_den*rhs_num;
        bool result = false;
        if(res_num != res_den){
            result = true;
        }
        return result;
    }

    bool operator>(const Fraction& lhs, const Fraction& rhs){
        int lhs_num = lhs.getNumerator();
        int lhs_den = lhs.getDenominator();
        int rhs_num = rhs.getNumerator();
        int rhs_den = rhs.getDenominator();
        int res_num = lhs_num*rhs_den;
        int res_den = lhs_den*rhs_num;
        bool result = false;
        if(res_num > res_den){
            result = true;
        }
        return result;
    }

    bool operator>=(const Fraction& lhs, const Fraction& rhs){
        int lhs_num = lhs.getNumerator();
        int lhs_den = lhs.getDenominator();
        int rhs_num = rhs.getNumerator();
        int rhs_den = rhs.getDenominator();
        int res_num = lhs_num*rhs_den;
        int res_den = lhs_den*rhs_num;
        bool result = false;
        if(res_num >= res_den){
            result = true;
        }
        return result;
    }

    bool operator<(const Fraction& lhs, const Fraction& rhs){
        int lhs_num = lhs.getNumerator();
        int lhs_den = lhs.getDenominator();
        int rhs_num = rhs.getNumerator();
        int rhs_den = rhs.getDenominator();
        int res_num = lhs_num*rhs_den;
        int res_den = lhs_den*rhs_num;
        bool result = false;
        if(res_num < res_den){
            result = true;
        }
        return result;
    }

    bool operator<=(const Fraction& lhs, const Fraction& rhs){
        int lhs_num = lhs.getNumerator();
        int lhs_den = lhs.getDenominator();
        int rhs_num = rhs.getNumerator();
        int rhs_den = rhs.getDenominator();
        int res_num = lhs_num*rhs_den;
        int res_den = lhs_den*rhs_num;
        bool result = false;
        if(res_num <= res_den){
            result = true;
        }
        return result;
    }

    std::istream& operator>>(std::istream& input,Fraction& f){
        int numerator = 0;
        int denominator = 1;
        input>>numerator;
        char split_c = input.peek();
        if(split_c == '/'){
            input.get();
            // 满足分数形式，继续读取
            input>>denominator;
            if(denominator == 0){
                numerator = 0;
                denominator = 1;
            }
        }
        f = Fraction(numerator,denominator);        
        return input;
    }
    std::ostream& operator<<(std::ostream& output,const Fraction& f){
        output<<f.getNumerator()<<"/"<<f.getDenominator();
        return output;
    }
    std::ostream& operator<<(std::ostream& output,const Fraction&& f){
        output<<f.getNumerator()<<"/"<<f.getDenominator();
        return output;
    }
    
}



