#include "common.h"


bool isFloat(const char* str) {
	bool hasDot = false;
	int len = strlen(str);

	for (int i = ((str[0] == '-') ? 1 : 0); i < len; i++) {
		if (str[i] < '0' || str[i] > '9') {
			if (!hasDot) {
				if (str[i] == '.') hasDot = true;
				else return false;
			}
			else return false;
		}
	}

	return str[len - 1] != '.';
}

float str2float(const char* str) {
	if (strcmp(str, "true") == 0) return 1.0f; // False is handled by the !isFloat(str)
	if (!isFloat(str) || (str[0] == 0)) return 0.0f;
	return std::stof(str);
}

float str2float(std::string str) {
	if (strcmp(str.data(), "true") == 0) return 1.0f; // False is handled by the !isFloat(str)
	if (!isFloat(str.data()) || (str[0] == 0)) return 0.0f;
	return std::stof(str);
}

char* getCharsForString(std::string str) {
	int len = str.length();

	char* chr = new char[len + 1];
	chr[len] = 0;
	for (int i = 0; i < len; i++) {
		chr[i] = str[i];
	}

	return chr;
}


bool collisionRectangle(float l1, float t1, float r1, float b1, float l2, float t2, float r2, float b2) { // https://www.geeksforgeeks.org/find-two-rectangles-overlap/
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

bool collisionPolyPoint(SDL_FPoint* vertices, int vertCount, float x, float y) { // http://www.jeffreythompson.org/collision-detection/poly-point.php
	bool collision = false;

	// go through each of the vertices, plus
	// the next vertex in the list
	int next = 0;
	for (int current = 0; current < vertCount; current++) {

		// get next vertex in list
		// if we've hit the end, wrap around to 0
		next = current + 1;
		if (next == vertCount) next = 0;

		// get the PVectors at our current position
		// this makes our if statement a little cleaner
		SDL_FPoint vc = vertices[current];    // c for "current"
		SDL_FPoint vn = vertices[next];       // n for "next"

		// compare position, flip 'collision' variable
		// back and forth
		if (((vc.y >= y && vn.y < y) || (vc.y < y && vn.y >= y)) &&
			(x < (vn.x - vc.x) * (y - vc.y) / (vn.y - vc.y) + vc.x)) {
			collision = !collision;
		}
	}
	return collision;
}


bool collisionLineLine(float x1, float y1, float x2, float y2, float x3, float y3, float x4, float y4) {

	// calculate the direction of the lines
	float uA = ((x4 - x3) * (y1 - y3) - (y4 - y3) * (x1 - x3)) / ((y4 - y3) * (x2 - x1) - (x4 - x3) * (y2 - y1));
	float uB = ((x2 - x1) * (y1 - y3) - (y2 - y1) * (x1 - x3)) / ((y4 - y3) * (x2 - x1) - (x4 - x3) * (y2 - y1));

	// if uA and uB are between 0-1, lines are colliding
	if (uA >= 0 && uA <= 1 && uB >= 0 && uB <= 1) {
		return true;
	}
	return false;
}


bool collisionPolyLine(SDL_FPoint* vertices, int vertCount, float x1, float y1, float x2, float y2) {

	// go through each of the vertices, plus the next
	// vertex in the list
	int next = 0;
	for (int current = 0; current < vertCount; current++) {

		// get next vertex in list
		// if we've hit the end, wrap around to 0
		next = current + 1;
		if (next == vertCount) next = 0;

		// get the PVectors at our current position
		// extract X/Y coordinates from each
		float x3 = vertices[current].x;
		float y3 = vertices[current].y;
		float x4 = vertices[next].x;
		float y4 = vertices[next].y;

		// do a Line/Line comparison
		// if true, return 'true' immediately and
		// stop testing (faster)
		bool hit = collisionLineLine(x1, y1, x2, y2, x3, y3, x4, y4);
		if (hit) {
			return true;
		}
	}

	// never got a hit
	return false;
}


bool collisionPolyPoly(SDL_FPoint* verts1, int verts1Count, SDL_FPoint* verts2, int verts2Count) { // http://www.jeffreythompson.org/collision-detection/poly-poly.php

	// go through each of the vertices, plus the next
	// vertex in the list
	int next = 0;
	for (int current = 0; current < verts1Count; current++) {

		// get next vertex in list
		// if we've hit the end, wrap around to 0
		next = current + 1;
		if (next == verts1Count) next = 0;

		// get the PVectors at our current position
		// this makes our if statement a little cleaner
		SDL_FPoint vc = verts1[current];    // c for "current"
		SDL_FPoint vn = verts1[next];       // n for "next"

		// now we can use these two points (a line) to compare
		// to the other polygon's vertices using polyLine()
		bool collision = collisionPolyLine(verts2, verts2Count, vc.x, vc.y, vn.x, vn.y);
		if (collision) return true;

		// optional: check if the 2nd polygon is INSIDE the first
		collision = collisionPolyPoint(verts1, verts1Count, verts2[0].x, verts2[0].y);
		if (collision) return true;
	}

	return false;
}	

bool fileExists(const char* name) {
	struct stat buffer;
	return (stat(name, &buffer) == 0);
}


const char* getFilenameFromPath(const char* path) {
	char* argf = (char*)path;
	char* filename = argf;
	while (*argf) {
		if (*argf == '/' || *argf == '\\')
			filename = argf + 1;

		argf++;
	}

	return (const char*)filename;
}


int fixWindowsPath(char* inPath, const char* path) {
	int idxO = 0;

	for (int idx = 0; idx < strlen(path); idx++) {
		if(inPath)
			inPath[idxO] = path[idx];

		if (path[idx] == '\\') {
			idxO++;
			if (inPath)
				inPath[idxO] = path[idx];
		}

		idxO++;
	}

	if (inPath)
		inPath[idxO] = 0;


	return idxO;
}

int countChar(char* text, char chr) {
	int len = strlen(text);

	int cnt = 0;
	for (int i = 0; i < len; i++) {
		if (text[i] == chr)
			cnt++;
	}

	return cnt;
}

int getRelativePath(char* output, char* pathA, char* pathB) {
	int pathALen = strlen(pathA);
	int pathBLen = strlen(pathB);

	int smallest = std::min(pathALen, pathBLen);
	int common;
	for (common = 0; common < smallest; common++)
	{
		if (pathA[common] != pathB[common])
			break;
	}

	if (common == 0) {
		if (output) {
			for (int i = 0; i < pathBLen; i++)
				output[i] = pathB[i];

			output[pathBLen] = 0;
		}

		return pathBLen;
	}

	pathA += common;
	pathB += common;

	while (pathA[0] == '\\')
		pathA++;

	while (pathB[0] == '\\')
		pathB++;

	int nbBack = (pathA[0] != 0) ? (countChar(pathA, '\\') + 1) : 0;

	int idx = 0;
	for (int i = 0; i < nbBack; i++) {
		if (output) {
			output[idx] = '.';
			output[idx] = '.';
			output[idx] = '/';
		}

		idx += 3;
	}
	
	pathBLen = strlen(pathB);
	for (int i = 0; i < pathBLen; i++) {
		if (output)
			output[idx] = (pathB[i] == '\\') ? '/' : pathB[i];

		idx++;
	}

	if (output)
		output[idx] = 0;

	return idx;
}


#include <direct.h>
#include <windows.h>
#include <stringapiset.h>
void GetCurrentDirW(char* _DstBuf, int _SizeInBytes) {
	wchar_t* wbuf = new wchar_t[_SizeInBytes];
	_wgetcwd(wbuf, _SizeInBytes);

	WideCharToMultiByte(CP_UTF8, 0, wbuf, -1, _DstBuf, _SizeInBytes, 0, 0);
}