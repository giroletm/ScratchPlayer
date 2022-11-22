#pragma once

#include <iostream>

#include "common.h"

#include "SDL.h"
#include "SDL_image.h"

#include "InputHandler.h";


class Executor;

class Game {
public:
	Game();
	~Game();

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

private:
	bool isRunning = false;
	u32 frameCount = 0;

public:
	SDL_Window* window;
	SDL_Renderer* renderer;
};