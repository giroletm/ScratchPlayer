#include "InputHandler.h"
#include "Game.h"

InputHandler::InputHandler() {
	keyHandles.push_back(new KeyHandle);
	keyHandles.push_back(new KeyHandle);
	keyHandles[0]->keycode = (SDL_Keycode)0; // All 0s (used for nonexistent keys)
	keyHandles[1]->keycode = (SDL_Keycode)0; // Any key state

	for (int i = 0; i < 3; i++) {
		mouseHandle.buttons[i] = new KeyHandle();
		mouseHandle.buttons[i]->keycode = i;
	}
}

InputHandler::~InputHandler() {
	u32 khsSize = keyHandles.size();
	for (u32 i = 0; i < khsSize; i++) {
		delete keyHandles[i];
	}
}

void InputHandler::updateMouse() {
	int xM, yM;
	SDL_GetMouseState(&xM, &yM);


	int ww, wh;
	SDL_GetWindowSize(Game::instance->window, &ww, &wh);

	int rRH = wh;
	int rRW = (int)ceil((float)wh * 800.0f / 600.0f);
	if (rRW > ww) {
		rRW = ww;
		rRH = (int)ceil((float)ww * 600.0f / 800.0f);
	}

	xM -= (ww / 2) - (rRW / 2);
	yM -= (wh / 2) - (rRH / 2);

	xM = (int)round((float)xM / ((float)rRW / 800.0f));
	yM = (int)round((float)yM / ((float)rRH / 600.0f));


	mouseHandle.x = xM;
	mouseHandle.y = yM;
}

void InputHandler::keyStateChanged(SDL_Keycode keycode, bool idUp) {
	// Blacklist
	if (keycode >= SDLK_F1 && keycode <= SDLK_F12) return;
	if (keycode == SDLK_CAPSLOCK || keycode == SDLK_LCTRL || keycode == SDLK_RCTRL ||
		keycode == SDLK_LGUI || keycode == SDLK_RGUI || keycode == SDLK_TAB || keycode == SDLK_LSHIFT ||
		keycode == SDLK_RSHIFT || keycode == SDLK_LALT || keycode == SDLK_RALT ||
		keycode == SDLK_NUMLOCKCLEAR || keycode == SDLK_VOLUMEDOWN || keycode == SDLK_VOLUMEUP || keycode == SDLK_CALCULATOR ||
		keycode == SDLK_PRINTSCREEN || keycode == SDLK_PAUSE || keycode == SDLK_PASTE || keycode == SDLK_BACKSPACE)
		return;

	// Aliases
	if (keycode == SDLK_KP_ENTER) keycode = SDLK_RETURN;
	if (keycode == SDLK_APPLICATION) keycode = SDL_BUTTON_RIGHT;
	if (keycode == SDLK_KP_0) keycode = SDLK_0;
	if (keycode >= SDLK_KP_1 && keycode <= SDLK_KP_9) keycode -= (SDLK_KP_1 - SDLK_1);

	KeyHandle* khandle = getKeyHandle(keycode, true);
	if (!khandle) {
		khandle = new KeyHandle;
		khandle->keycode = keycode;
		keyHandles.push_back(khandle);
	}

	if (!idUp) {
		khandle->pressed = true;
		keyHandles[1]->pressed = true;
		khandle->held = true;
		khandle->released = false;
	}
	else {
		if (khandle->held)
			khandle->released = true;
		khandle->pressed = false;
		khandle->held = false;
	}
}

const char* keyNames[] = {
	"space",
	"up arrow",
	"down arrow",
	"right arrow",
	"left arrow",
	"any",
	"a",
	"b",
	"c",
	"d",
	"e",
	"f",
	"g",
	"h",
	"i",
	"j",
	"k",
	"l",
	"m",
	"n",
	"o",
	"p",
	"q",
	"r",
	"s",
	"t",
	"u",
	"v",
	"w",
	"x",
	"y",
	"z",
	"0",
	"1",
	"2",
	"3",
	"4",
	"5",
	"6",
	"7",
	"8",
	"9"
};

const SDL_Keycode keyIDs[] = {
	SDLK_SPACE,
	SDLK_UP,
	SDLK_DOWN,
	SDLK_RIGHT,
	SDLK_LEFT,
	0,
	SDLK_a,
	SDLK_b,
	SDLK_c,
	SDLK_d,
	SDLK_e,
	SDLK_f,
	SDLK_g,
	SDLK_h,
	SDLK_i,
	SDLK_j,
	SDLK_k,
	SDLK_l,
	SDLK_m,
	SDLK_n,
	SDLK_o,
	SDLK_p,
	SDLK_q,
	SDLK_r,
	SDLK_s,
	SDLK_t,
	SDLK_u,
	SDLK_v,
	SDLK_w,
	SDLK_x,
	SDLK_y,
	SDLK_z,
	SDLK_0,
	SDLK_1,
	SDLK_2,
	SDLK_3,
	SDLK_4,
	SDLK_5,
	SDLK_6,
	SDLK_7,
	SDLK_8,
	SDLK_9,
};

KeyHandle* InputHandler::getKeyHandleByName(const char* name, bool return0) {
	int keyLen = (sizeof(keyNames) / sizeof(const char*));

	int i = 0;
	while (i < keyLen) {
		if (strcmp(keyNames[i], name) == 0) break;

		i++;
	}

	SDL_Keycode kc = keyIDs[i];
	if (kc == 0) return getAnyKeyHandle();
	else return getKeyHandle(kc, return0);
}

KeyHandle* InputHandler::getKeyHandle(SDL_Keycode keycode, bool return0) {
	if (keycode >= SDL_BUTTON_LEFT && keycode <= SDL_BUTTON_RIGHT) {
		return mouseHandle.buttons[keycode - 1];
	}

	u32 khsSize = keyHandles.size();
	for (u32 i = 2; i < khsSize; i++) {
		if (keyHandles[i]->keycode == keycode) return keyHandles[i];
	}

	if (return0) return 0;
	else return keyHandles[0];
}

KeyHandle* InputHandler::getAnyKeyHandle() {
	keyHandles[1]->held = false;
	keyHandles[1]->released = false;

	u32 khsSize = keyHandles.size();
	for (u32 i = 2; i < khsSize; i++) {
		if (keyHandles[i]->held) {
			keyHandles[1]->held = true;
			break;
		}
	}

	if (!keyHandles[1]->held) {
		for (u32 i = 2; i < khsSize; i++) {
			if (keyHandles[i]->released) {
				keyHandles[1]->released = true;
				break;
			}
		}
	}

	return keyHandles[1];
}

void InputHandler::updateKeys() {
	for (int i = 0; i < 3; i++) {
		mouseHandle.buttons[i]->pressed = false;
		mouseHandle.buttons[i]->released = false;
	}

	int khsSize = keyHandles.size();
	for (int i = 0; i < khsSize; i++) {
		keyHandles[i]->pressed = false;
		keyHandles[i]->released = false;
	}
}