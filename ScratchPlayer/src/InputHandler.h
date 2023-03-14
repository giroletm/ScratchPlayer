#pragma once

#include "common.h"
#include <vector>

#include "SDL_keycode.h"
#include "SDL_mouse.h"


class KeyHandle {
public:
	SDL_Keycode keycode;
	bool pressed = 0;
	bool held = 0;
	bool released = 0;
};

class MouseHandle {
public:
	int x = 0;
	int y = 0;
	KeyHandle* buttons[3];
};

class InputHandler {
public:
	InputHandler();
	~InputHandler();

	MouseHandle mouseHandle;
	std::vector<KeyHandle*> keyHandles;

	void updateMouse();

	void keyStateChanged(SDL_Keycode keycode, bool idUp);
	KeyHandle* getKeyHandle(SDL_Keycode keycode, bool return0 = false);
	KeyHandle* getKeyHandleByName(const char* name, bool return0 = false);
	KeyHandle* getAnyKeyHandle();
	void updateKeys();
};