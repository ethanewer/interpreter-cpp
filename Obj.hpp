#ifndef OBJ
#define OBJ

#include <string>
#include <vector>
#include "Obj.hpp"

class Obj {
public:
    Obj() {} 
    
    virtual ~Obj() {}
};

class BoolObj : public Obj {
public:
    bool val;
    
    BoolObj(bool val) : val(val) {}

    Obj* clone() {
        return new BoolObj(val);
    } 
};

class DoubleObj : public Obj {
public:
    double val;
    
    DoubleObj(double val) : val(val) {}
};

class StringObj : public Obj {
public:
    std::string val;
    
    StringObj(std::string val) : val(val) {}
};

#endif