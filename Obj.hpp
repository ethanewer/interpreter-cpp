#ifndef OBJ
#define OBJ

#include <string>
#include <vector>
#include <unordered_map>
#include <functional>

struct Obj {
    Obj();

    std::string to_string();

    size_t hash();
    
    virtual ~Obj() {}
};

struct BoolObj : public Obj {
    bool val;
    
    BoolObj(bool val);

    std::string to_string();
};

struct DoubleObj : public Obj {
    double val;
    
    DoubleObj(double val);

    std::string to_string();
};

struct StringObj : public Obj {
    std::string val;
    
    StringObj(std::string val);

    std::string to_string();
};

#endif