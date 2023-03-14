#include "Game.h"
#include "Executor.h"

Game* Game::instance = 0;

Game::Game(int argc, char* argv[]) {
	instance = this;

	this->argc = argc;
	this->argv = argv;

	rte = 0;
	rteTex = 0;
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

		if (TTF_Init() != 0) return -4; // Couldn't init SDL_TTF

		isRunning = true;

		if (argc != 2)
			rte = 1;
		else {
			// Windows paths SUCKS, and accented paths cause issues.
			// The point of this code snippet is to convert absolute paths to relative ones to avoid accents as much as possible, and to adapt forcefully absolute paths to something C++ can accept.
			char* fixedPath = NULL;
			if ((argv[1])[1] == ':') { // Check if the path inputed is an absolute windows path
				// Get working directory
				char buff[FILENAME_MAX];
				GetCurrentDir(buff, FILENAME_MAX);

				// Make path relative if possible
				int relativeSize = getRelativePath(NULL, buff, argv[1]);
				char* relativePath = new char[relativeSize+1];
				getRelativePath(relativePath, buff, argv[1]);

				// If the path couldn't be rewritten as a relative path, fix windows backslashes
				if ((argv[1])[1] == ':') {
					int fixedSize = fixWindowsPath(NULL, relativePath);
					fixedPath = new char[fixedSize + 1];
					fixWindowsPath(fixedPath, relativePath);

					delete[] relativePath;
				}
				else {
					fixedPath = relativePath;
				}
			}
			else {
				fixedPath = argv[1];
			}

			#ifdef _DEBUG
			printf("Opening %s\n", fixedPath);
			#endif

			if (!fileExists(fixedPath))
				rte = 2;
			else
				exec = new Executor(fixedPath);

			if(fixedPath != argv[1])
				delete[] fixedPath;
		}

		return 0;
	}
	else {
		return -1; // Couldn't init SDL2
	}
}

void Game::handleEvents() {
	inputHandler.updateKeys();

	SDL_Event event;
	SDL_PollEvent(&event);

	inputHandler.updateMouse();

	switch (event.type) {
		case SDL_QUIT: {
			isRunning = false;
			break;
		}
		case SDL_KEYDOWN: {
			inputHandler.keyStateChanged(event.key.keysym.sym, false);
			break;
		}
		case SDL_KEYUP: {
			inputHandler.keyStateChanged(event.key.keysym.sym, true);
			break;
		}
		case SDL_MOUSEBUTTONDOWN: {
			u32 buttonState = SDL_GetMouseState(NULL, NULL);

			if (buttonState & SDL_BUTTON(SDL_BUTTON_LEFT)) {
				inputHandler.keyStateChanged(SDL_BUTTON_LEFT, false);

				if(exec)
					exec->isRunning = true;
			}
			if (buttonState & SDL_BUTTON(SDL_BUTTON_MIDDLE))
				inputHandler.keyStateChanged(SDL_BUTTON_MIDDLE, false);
			if (buttonState & SDL_BUTTON(SDL_BUTTON_RIGHT))
				inputHandler.keyStateChanged(SDL_BUTTON_RIGHT, false);

			break;
		}
		case SDL_MOUSEBUTTONUP: {
			u32 buttonState = SDL_GetMouseState(NULL, NULL);

			if (!(buttonState & SDL_BUTTON(SDL_BUTTON_LEFT)))
				inputHandler.keyStateChanged(SDL_BUTTON_LEFT, true);
			if (!(buttonState & SDL_BUTTON(SDL_BUTTON_MIDDLE)))
				inputHandler.keyStateChanged(SDL_BUTTON_MIDDLE, true);
			if (!(buttonState & SDL_BUTTON(SDL_BUTTON_RIGHT)))
				inputHandler.keyStateChanged(SDL_BUTTON_RIGHT, true);

			break;
		}
		default: {
			break;
		}
	}
}

void Game::update() {
	frameCount++;

	if (exec)
		exec->executeBlocks();
}

void Game::render() {
	SDL_RenderClear(renderer);

	if (exec)
		exec->render();
	if (rte) {
		if (!rteTex) {

			u64 dFontSize;
			const u8* dFont = getDebugFont(&dFontSize);

			SDL_RWops* rwFont = SDL_RWFromMem((void*)dFont, dFontSize);
			TTF_Font* font = TTF_OpenFontRW(rwFont, 0, 24);
			TTF_SetFontStyle(font, TTF_STYLE_BOLD);

			SDL_Color color = { 0, 0, 0 };

			char errorMsg[1000];
			if (rte == 1) {
				sprintf_s(errorMsg, "Error:\nNo input sb3 file was specified.\nUse \"%s <filename>\"\nOr drag-and-drop the sb3 file on the executable.", getFilenameFromPath(argv[0]));
			}
			else if (rte == 2) {
				sprintf_s(errorMsg, "Error:\nFile \"%s\" couldn't be found.", getFilenameFromPath(argv[1]));
			}
			else {
				sprintf_s(errorMsg, "Error:\nUnhandled error");
			}
			SDL_Surface* surfaceMessage = TTF_RenderText_Blended_Wrapped(font, errorMsg, color, 0);

			// now you can convert it into a texture
			rteTex = SDL_CreateTextureFromSurface(renderer, surfaceMessage);

			SDL_SetWindowSize(window, surfaceMessage->w, surfaceMessage->h);

			SDL_FreeSurface(surfaceMessage);
		}

		SDL_Rect size;
		size.x = 0;
		size.y = 0;
		SDL_QueryTexture(rteTex, NULL, NULL, &size.w, &size.h);

		SDL_RenderCopy(renderer, rteTex, NULL, &size);
	}

	SDL_RenderPresent(renderer);
}

void Game::clean() {
	SDL_DestroyWindow(window);
	SDL_DestroyRenderer(renderer);
	SDL_Quit();
}