#pragma once

#include <iostream>

#include "common.h"
#include "resources.h"

#include "SDL.h"
#include "SDL_image.h"
#include "SDL_ttf.h"

#include "InputHandler.h";


class Executor;

class Game {
public:
	Game(int argc, char* argv[]);
	~Game();

	int argc;
	char** argv;
	int rte;
	SDL_Texture* rteTex;

	Executor* exec;
	InputHandler inputHandler;

	static Game* instance;

	int init(const char* title, int xpos, int ypos, int width, int height, u32 flags);

	void handleEvents();
	void update();
	void render();
	void clean();

	bool running() { return isRunning; }
	u32 getFrameCount() { return frameCount; }
	void resetFrameCount() { frameCount = 0; }

private:
	bool isRunning = false;
	u32 frameCount = 0;

public:
	SDL_Window* window;
	SDL_Renderer* renderer;
};