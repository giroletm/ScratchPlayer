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

	bool isRunning;
	SDL_Texture* hangOn;

	void render();
	void executeBlocks();

	void triggerBackdropSwitch(const char* name);
	void triggerBroadcast(const char* uniqueID);
	bool isBroadcastOn(const char* uniqueID);

	void stopBlockSets(BlockSet* bs=0, Sprite* s=0);
};