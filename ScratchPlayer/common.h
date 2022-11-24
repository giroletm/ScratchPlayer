#pragma once

#include <iostream>
#include <sstream>

#include "json.hpp"
using json = nlohmann::json;


typedef unsigned char u8;
typedef unsigned short u16;
typedef unsigned int u32;
typedef unsigned long u64;


template <typename T>
std::string tostr(const T& t) {
	std::ostringstream os;
	os << t;
	return os.str();
}

bool isFloat(const char* str);

float str2float(const char* str);
float str2float(std::string str);

char* getCharsForString(std::string str);
inline char* getCharsForJSON(json data) {
	if (data.is_null()) return 0;

	return getCharsForString(data);
}