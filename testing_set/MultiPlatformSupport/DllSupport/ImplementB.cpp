#include "Interfaces.h"
class ImplementRetB:public Interface{
public:
    ImplementRetB():Interface(){};
    virtual std::string getValue()override{
        return "B";
    }
    virtual ~ImplementRetB(){};
};

std::shared_ptr<Interface> createInstance(){
    return std::make_shared<ImplementRetB>();
}

void destroyInstance(Interface* p){
    delete p;
}