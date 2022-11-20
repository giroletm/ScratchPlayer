#pragma once

#include "ZipFile.h"
#include "Targets.h"

class Executor {
public:
	Executor(const char* sb3Filename);
	~Executor();

	ZipFile* zfile;
	Targets* targets;

	static Executor* instance;

	void render();
};