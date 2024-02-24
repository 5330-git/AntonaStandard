#ifndef FUNCS_H
#define FUNCS_H

#include <CPS/Dll.h>

extern "C"{
    ASD_EXPORT double add_db(double,double);
    ASD_EXPORT double sub_db(double,double);
    ASD_EXPORT double multi_db(double,double);
    
    ASD_EXPORT double div_db(double,double);
    ASD_EXPORT void sayHello();

}
#endif