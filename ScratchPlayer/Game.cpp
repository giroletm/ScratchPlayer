#include "Game.h"
#include "Executor.h"

Game* Game::instance = 0;

Game::Game() {
	instance = this;

	exec = 0;
}

Game::~Game() {
	if (exec) delete exec;

	instance = 0;
}

int Game::init(const char* title, int xpos, int ypos, int width, int height, u32 flags) {
	if (SDL_Init(SDL_INIT_EVERYTHING) == 0) {

		window = SDL_CreateWindow(title, xpos, ypos, width, height, flags);
		if (!window) return -2; // Coudln't create window

		renderer = SDL_CreateRenderer(window, -1, 0 | SDL_RENDERER_TARGETTEXTURE);
		if (!renderer) return -3; // Coudln't create renderer

		SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
		SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);

		isRunning = true;

		exec = new Executor("Test3.sb3");

		return 0;
	}
	else {
		return -1; // Couldn't init SDL2
	}
}

void Game::handleEvents() {
	SDL_Event event;
	SDL_PollEvent(&event);

	switch (event.type) {
		case SDL_QUIT: {
			isRunning = false;
			break;
		}
		case SDL_MOUSEBUTTONDOWN: {
			if (SDL_GetMouseState(NULL, NULL) & SDL_BUTTON(SDL_BUTTON_LEFT)) exec->isRunning = true;
			break;
		}
		default: {
			break;
		}
	}
}

void Game::update() {
	frameCount++;
}

void Game::render() {
	SDL_RenderClear(renderer);

	exec->render();

	SDL_RenderPresent(renderer);
}

void Game::clean() {
	SDL_DestroyWindow(window);
	SDL_DestroyRenderer(renderer);
	SDL_Quit();
}