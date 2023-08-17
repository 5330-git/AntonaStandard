#ifndef FUNCS_H
#define FUNCS_H

#include "MultiPlatformSupport/DllSupport.h"

extern "C"{
    ASD_EXPORT_FUNC double add_db(double,double);
    ASD_EXPORT_FUNC double sub_db(double,double);
    ASD_EXPORT_FUNC double multi_db(double,double);
    
    ASD_EXPORT_FUNC double div_db(double,double);
    ASD_EXPORT_FUNC void sayHello();

}
#endif