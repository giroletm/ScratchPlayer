#include <iostream>
#include <string>

#include "Sprites.h"


char* getCharsForString(json data) {
	if (data.is_null()) return 0;

	std::string str = data;

	int len = str.length();

	char* chr = new char[len + 1];
	chr[len] = 0;
	for (int i = 0; i < len; i++) {
		chr[i] = str[i];
	}

	return chr;
}


Targets::Targets(json content) {
	data = content["targets"];

	int spriteCount = data.size();
	#ifdef _DEBUG
	printf("Found %d sprites.\n", spriteCount);
	#endif

	this->sprites = new Sprite*[spriteCount];
	for (int i = 0; i < spriteCount; i++) {
		this->sprites[i] = new Sprite(data[i]);
	}
}

Sprite::Sprite(json data) {
	this->isStage = data["isStage"];
	this->name = getCharsForString(data["name"]);

	this->currentCostume = data["currentCostume"];
	this->volume = data["volume"];
	this->layerOrder = data["layerOrder"];

	if (this->isStage) {
		this->tempo = data["tempo"];
		this->videoTransparency = data["videoTransparency"];
		this->videoState = getCharsForString(data["videoState"]);
		this->textToSpeechLanguage = getCharsForString(data["textToSpeechLanguage"]);
	}
	else {
		this->visible = data["visible"];
		this->x = data["x"];
		this->y = data["y"];
		this->size = data["size"];
		this->direction = data["direction"];
		this->draggable = data["draggable"];
		this->rotationStyle = getCharsForString(data["rotationStyle"]);
	}
}


Targets::~Targets() {
	int spriteCount = data.size();
	for (int i = 0; i < spriteCount; i++) {
		delete this->sprites[i];
	}

	delete[] this->sprites;
}

Sprite::~Sprite() {
	delete[] this->name;

	if (this->isStage) {
		if(this->videoState)
			delete[] this->videoState;

		if (this->textToSpeechLanguage)
			delete[] this->textToSpeechLanguage;
	}
	else {
		if (this->rotationStyle)
			delete[] this->rotationStyle;
	}
}


Sprite* Targets::getSpriteByName(const char* name) {
	return 0;
}



Variable* Sprite::getVariableByUniqueID(const char* uniqueID) {
	return 0;
}

List* Sprite::getListByUniqueID(const char* uniqueID) {
	return 0;
}

Broadcast* Sprite::getBroadcastByUniqueID(const char* uniqueID) {
	return 0;
}

Block* Sprite::getBlockByUniqueID(const char* uniqueID) {
	return 0;
}

Comment* Sprite::getCommentByUniqueID(const char* uniqueID) {
	return 0;
}

Costume* Sprite::getCostumeByName(const char* name) {
	return 0;
}

Sound* Sprite::getSoundByName(const char* name) {
	return 0;
}