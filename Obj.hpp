#ifndef OBJ
#define OBJ

#include <string>
#include <vector>
#include <unordered_map>

struct Obj {
    Obj() {} 

    std::string to_string() {
        return "<obj>";
    }
    
    virtual ~Obj() {}
};

struct BoolObj : public Obj {
    bool val;
    
    BoolObj(bool val) : val(val) {}

    std::string to_string() {
        return val ? "true" : "false";
    }
};

struct DoubleObj : public Obj {
    double val;
    
    DoubleObj(double val) : val(val) {}

    std::string to_string() {
        auto num_str = std::to_string(val);
		while (num_str.back() == '0') num_str.pop_back();
		if (num_str.back() == '.') num_str.pop_back();
        return num_str;
    }
};

struct StringObj : public Obj {
    std::string val;
    
    StringObj(std::string val) : val(val) {}

    std::string to_string() {
        return val;
    }
};

#endif