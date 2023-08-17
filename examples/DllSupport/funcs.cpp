#include "funcs.h"
#include <iostream>
double add_db(double a, double b) { 
    return  a+b;
}

double sub_db(double a, double b) { 
    return  a-b;
}
double multi_db(double a, double b) { 
    return  a*b;
}
double div_db(double a, double b) { 
    return  a/b;
}

void sayHello(){
    std::cout<<"Hello from dll func!"<<std::endl;
}
