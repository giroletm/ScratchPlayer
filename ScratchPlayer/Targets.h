#pragma once

#include <vector>;

#include "SDL.h";
#include "SDL_image.h";

#include "json.hpp";
using json = nlohmann::json;

#include "Game.h";


class Sprite;

class Variable;

class List;

class Broadcast;

class Block;
class Input;
class Field;

class Comment;

class Costume;

class Sound;

class Targets {
public:
	Targets(json content);
	~Targets();

	json data;
	Sprite** sprites;

	Sprite* getSpriteByName(const char* name);

	Variable* getVariableByUniqueID(const char* uniqueID, int spriteID);
	List* getListByUniqueID(const char* uniqueID, int spriteID);
	Broadcast* getBroadcastByUniqueID(const char* uniqueID, int spriteID);
};

class Sprite {
public:
	Sprite(json data);
	~Sprite();

	bool isStage;
	char* name;

	std::vector<Variable*> variables;
	std::vector<List*> lists;
	std::vector<Broadcast*> broadcasts;
	std::vector<Block*> blocks;
	std::vector<Comment*> comments;

	int currentCostume;
	std::vector<Costume*> costumes;

	std::vector<Sound*> sounds;
	int volume;

	int layerOrder;

	// Only if !isStage
	bool visible;
	float x;				// Stored as an int, but a float in-game
	float y;				// ^
	float size;				// ^
	float direction;		// ^
	bool draggable;
	char* rotationStyle;

	// Only if isStage
	int tempo;
	int videoTransparency;
	char* videoState;
	char* textToSpeechLanguage;


	Variable* getVariableByUniqueID(const char* uniqueID);
	List* getListByUniqueID(const char* uniqueID);
	Broadcast* getBroadcastByUniqueID(const char* uniqueID);
	Block* getBlockByUniqueID(const char* uniqueID);
	Comment* getCommentByUniqueID(const char* uniqueID);

	Costume* getCostumeByName(const char* name);
	Sound* getSoundByName(const char* name);
};

class Variable {
public:
	Variable(std::string id, json data);
	~Variable();

	char* uniqueID;

	char* name;
	char* value;
};

class List {
public:
	List(std::string id, json data);
	~List();

	char* uniqueID;

	char* name;
	std::vector<char*> values;
};

class Broadcast {
public:
	Broadcast(std::string id, json data);
	~Broadcast();

	char* uniqueID;

	char* name;
};

class Block {
public:
	Block(std::string id, json data);
	~Block();

	char* uniqueID;

	char* opcode;

	Block* next;
	Block* parent;

	std::vector<Input*> inputs;
	std::vector<Field*> fields;

	bool shadow;
	bool topLevel;

	int x;
	int y;

	void doParenting(Sprite* sprite, json data);
};

class Input {
public:
	Input(std::string id, json data);
	~Input();

	char* name;

	json content;
};

class Field {
public:
	Field(std::string id, json data);
	~Field();

	char* name;

	json content;
};

class Comment {
public:
	Comment(std::string id, json data, Sprite* sprite);
	~Comment();

	char* uniqueID;

	Block* blockId;

	float x;
	float y;

	int width;
	int height;

	bool minimized;

	char* text;
};

class Asset {
public:
	Asset(json data);
	~Asset();

	char* name;
	char* dataFormat;
	char* assetId;
	char* md5ext;
};

class Costume : public Asset {
public:
	Costume(json data);
	~Costume();

	int bitmapResolution;

	int rotationCenterX;
	int rotationCenterY;

	SDL_Texture* costumeTexture;
};

class Sound : public Asset {
public:
	Sound(json data);
	~Sound();

	char* format;
	int rate;
	int sampleCount;
};