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