#pragma once

#include <iostream>
#include <sstream>

#include "json.hpp"
using json = nlohmann::json;

#include <SDL_rect.h>


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

bool collisionRectangle(float l1, float t1, float r1, float b1, float l2, float t2, float r2, float b2); // https://www.geeksforgeeks.org/find-two-rectangles-overlap/
bool collisionPolyPoint(SDL_FPoint* vertices, int vertCount, float x, float y); // http://www.jeffreythompson.org/collision-detection/poly-point.php
bool collisionLineLine(float x1, float y1, float x2, float y2, float x3, float y3, float x4, float y4);
bool collisionPolyLine(SDL_FPoint* vertices, int vertCount, float x1, float y1, float x2, float y2);
bool collisionPolyPoly(SDL_FPoint* verts1, int verts1Count, SDL_FPoint* verts2, int verts2Count); // http://www.jeffreythompson.org/collision-detection/poly-poly.php


inline bool onEdge(float x, float y) {
	return !collisionPoint(-240, 180, 240, -180, x, y);
}

bool fileExists(const char* name);

const char* getFilenameFromPath(const char* path);

int fixWindowsPath(char* inPath, const char* path);

int getRelativePath(char* output, char* pathA, char* pathB);

#ifdef _WIN32
void GetCurrentDirW(char* _DstBuf, int _SizeInBytes);
#define GetCurrentDir GetCurrentDirW
#else
#include <unistd.h>
#define GetCurrentDir getcwd
#endif