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

inline float distance(int x1, int y1, int x2, int y2) {
	float dX = (x2 - x1);
	float dY = (y2 - y1);
	return sqrtf((dX * dX) + (dY * dY));
}

inline bool collisionPoint(float left, float top, float right, float bottom, float x, float y) {
	return ((x >= left && x <= right) && (y >= bottom && y <= top));
}

inline bool collisionRectangle(float l1, float t1, float r1, float b1, float l2, float t2, float r2, float b2) { // https://www.geeksforgeeks.org/find-two-rectangles-overlap/
	// if rectangle has area 0, no overlap
	if (l1 == r1 || t1 == b1 || r2 == l2 || t2 == b2)
		return false;

	// If one rectangle is on left side of other
	if (l1 > r2 || l2 > r1)
		return false;

	// If one rectangle is above other
	if (b1 > t2 || b2 > t1)
		return false;

	return true;
}

inline bool onEdge(float x, float y) {
	return !collisionPoint(-240, 180, 240, -180, x, y);
}