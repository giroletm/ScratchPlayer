#include "Game.h"

int main(int argc, char* argv[]) {
	srand(time(NULL));

	const int targetFPS = 60;
	const int frameDelay = 1000 / targetFPS;

	// Init
	Game* game = new Game();

	int errInit = game->init("ScratchPlayer", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 480, 360, SDL_WINDOW_SHOWN);

	if (errInit == 0) {
		while (game->running()) {
			u32 frameStart = SDL_GetTicks();

			// Events & Inputs
			game->handleEvents();

			// Updates
			game->update();

			// Render
			game->render();

			u32 frameTime = SDL_GetTicks() - frameStart;

			if (frameDelay > frameTime) SDL_Delay(frameDelay - frameTime);
		}

		game->clean();
	}
	#ifdef _DEBUG
	else printf("Couldn't initialize SDL2: Error %d\n", errInit);
	#endif

	delete game;


	return 0;
}