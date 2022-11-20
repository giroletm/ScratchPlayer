#include <iostream>
#include <string>

#include "Sprites.h"


char* getCharsForString(std::string str) {
	int len = str.length();

	char* chr = new char[len + 1];
	chr[len] = 0;
	for (int i = 0; i < len; i++) {
		chr[i] = str[i];
	}

	return chr;
}

char* getCharsForJSON(json data) {
	if (data.is_null()) return 0;

	return getCharsForString(data);
}


Targets::Targets(json content) {
	data = content["targets"];

	int spriteCount = data.size();
	#ifdef _DEBUG
	printf("Found %d sprites.\n", spriteCount);
	#endif

	this->sprites = new Sprite*[spriteCount];
	for (int i = 0; i < spriteCount; i++) {
		#ifdef _DEBUG
		printf("Sprite %d:\n", i);
		#endif
		this->sprites[i] = new Sprite(data[i]);
	}
}

Sprite::Sprite(json data) {
	this->isStage = data["isStage"];
	this->name = getCharsForJSON(data["name"]);

	this->currentCostume = data["currentCostume"];
	this->volume = data["volume"];
	this->layerOrder = data["layerOrder"];

	if (this->isStage) {
		this->tempo = data["tempo"];
		this->videoTransparency = data["videoTransparency"];
		this->videoState = getCharsForJSON(data["videoState"]);
		this->textToSpeechLanguage = getCharsForJSON(data["textToSpeechLanguage"]);
	}
	else {
		this->visible = data["visible"];
		this->x = data["x"];
		this->y = data["y"];
		this->size = data["size"];
		this->direction = data["direction"];
		this->draggable = data["draggable"];
		this->rotationStyle = getCharsForJSON(data["rotationStyle"]);
	}

	json varsData = data["variables"];
	json listsData = data["lists"];
	json broadcastsData = data["broadcasts"];
	json blocksData = data["blocks"];
	json commentsData = data["comments"];
	json costumesData = data["costumes"];
	json soundsData = data["sounds"];

	int varsCount = varsData.size();
	int listsCount = listsData.size();
	int broadcastsCount = broadcastsData.size();
	int blocksCount = blocksData.size();
	int commentsCount = commentsData.size();
	int costumesCount = costumesData.size();
	int soundsCount = soundsData.size();


	#ifdef _DEBUG
	printf("\tFound %d variables.\n", varsCount);
	#endif
	for (auto i = varsData.begin(); i != varsData.end(); i++) {
		this->variables.push_back(new Variable(i.key(), i.value()));
	}

	#ifdef _DEBUG
	printf("\tFound %d lists.\n", listsCount);
	#endif
	for (auto i = listsData.begin(); i != listsData.end(); i++) {
		this->lists.push_back(new List(i.key(), i.value()));
	}

	#ifdef _DEBUG
	printf("\tFound %d broadcasts.\n", broadcastsCount);
	#endif
	for (auto i = broadcastsData.begin(); i != broadcastsData.end(); i++) {
		this->broadcasts.push_back(new Broadcast(i.key(), i.value()));
	}

	#ifdef _DEBUG
	printf("\tFound %d blocks.\n", blocksCount);
	#endif
	for (auto i = blocksData.begin(); i != blocksData.end(); i++) {
		this->blocks.push_back(new Block(i.key(), i.value()));
	}
	int j = 0;
	for (auto i = blocksData.begin(); i != blocksData.end(); i++) {
		this->blocks[j]->doParenting(this, i.value());
		j++;
	}

	#ifdef _DEBUG
	printf("\tFound %d comments.\n", commentsCount);
	#endif
	for (auto i = commentsData.begin(); i != commentsData.end(); i++) {
		this->comments.push_back(new Comment(i.key(), i.value(), this));
	}

	#ifdef _DEBUG
	printf("\tFound %d costumes.\n", costumesCount);
	#endif
	for (int i = 0; i < costumesCount; i++) {
		this->costumes.push_back(new Costume(costumesData[i]));
	}

	#ifdef _DEBUG
	printf("\tFound %d sounds.\n", soundsCount);
	#endif
	for (int i = 0; i < soundsCount; i++) {
		this->sounds.push_back(new Sound(soundsData[i]));
	}
}

Variable::Variable(std::string id, json data) {

}

List::List(std::string id, json data) {

}

Broadcast::Broadcast(std::string id, json data) {

}

Block::Block(std::string id, json data) {

}

Input::Input(std::string id, json data) {

}

Field::Field(std::string id, json data) {

}

Comment::Comment(std::string id, json data, Sprite* sprite) {

}

Asset::Asset(json data) {

}

Costume::Costume(json data) : Asset(data) {

}

Sound::Sound(json data) : Asset(data) {

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
	
	int varsCount = this->variables.size();
	int listsCount = this->lists.size();
	int broadcastsCount = this->broadcasts.size();
	int blocksCount = this->blocks.size();
	int commentsCount = this->comments.size();
	int costumesCount = this->costumes.size();
	int soundsCount = this->sounds.size();

	for (int i = 0; i < varsCount; i++) 
		delete this->variables[i];

	for (int i = 0; i < listsCount; i++)
		delete this->lists[i];

	for (int i = 0; i < broadcastsCount; i++)
		delete this->broadcasts[i];

	for (int i = 0; i < blocksCount; i++)
		delete this->blocks[i];

	for (int i = 0; i < commentsCount; i++)
		delete this->comments[i];

	for (int i = 0; i < costumesCount; i++)
		delete this->costumes[i];

	for (int i = 0; i < soundsCount; i++)
		delete this->sounds[i];
}

Variable::~Variable() {

}

List::~List() {

}

Broadcast::~Broadcast() {

}

Block::~Block() {

}

Input::~Input() {

}

Field::~Field() {

}

Comment::~Comment() {

}

Asset::~Asset() {

}

Costume::~Costume() {

}

Sound::~Sound() {

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


void Block::doParenting(Sprite* sprite, json data) {
	char* pName = getCharsForJSON(data["parent"]);
	char* nName = getCharsForJSON(data["next"]);

	if (pName) {
		this->parent = sprite->getBlockByUniqueID(pName);
		delete[] pName;
	}
	else this->parent = 0;

	if (nName) {
		this->next = sprite->getBlockByUniqueID(nName);
		delete[] nName;
	}
	else this->next = 0;
}