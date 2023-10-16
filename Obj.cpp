#include "Obj.hpp"

Obj::Obj() {} 

std::string Obj::to_string() {
	return "<obj>";
}

size_t Obj::hash() {
		if (auto obj = dynamic_cast<BoolObj*>(this)) {
		std::hash<bool> hasher;
		return hasher(obj->val);
	}
	if (auto obj = dynamic_cast<DoubleObj*>(this)) {
		std::hash<double> hasher;
		return hasher(obj->val);
	}
	if (auto obj = dynamic_cast<StringObj*>(this)) {
		std::hash<std::string> hasher;
		return hasher(obj->val);
	}
	std::hash<Obj*> hasher;
	return hasher(this);
}

BoolObj::BoolObj(bool val) : val(val) {}

std::string BoolObj::to_string() {
	return val ? "true" : "false";
}

DoubleObj::DoubleObj(double val) : val(val) {}

std::string DoubleObj::to_string() {
	auto num_str = std::to_string(val);
	while (num_str.back() == '0') num_str.pop_back();
	if (num_str.back() == '.') num_str.pop_back();
	return num_str;
}

StringObj::StringObj(std::string val) : val(val) {}

std::string StringObj::to_string() {
	return val;
}
