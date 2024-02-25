#include "Interfaces.h"
class ImplementRetA:public Interface{
public:
    ImplementRetA():Interface(){};
    virtual std::string getValue()override{
        return "A";
    }
    virtual ~ImplementRetA(){};
};

std::shared_ptr<Interface> createInstance(){
    return std::make_shared<ImplementRetA>();
}
void destroyInstance(Interface* p){
    delete p;
}
