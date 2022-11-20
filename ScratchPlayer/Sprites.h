#pragma once

#include <vector>;

#include "json.hpp";
using json = nlohmann::json;


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

	Sprite* sprites;

	Sprite* getSpriteByName(const char* name);
};

class Sprite {
public:
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
	int tempo;

	int layerOrder;

	// Only if !isStage
	bool visible;
	int x;
	int y;
	int size;
	int direction;
	bool draggable;
	char* rotationStyle;

	// Only if isStage
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
	char* uniqueID;

	char* name;
	char* value;
};

class List {
public:
	char* uniqueID;

	char* name;
	std::vector<char*> values;
};

class Broadcast {
public:
	char* uniqueID;

	char* name;
};

class Block {
public:
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
};

class Input {
public:
	char* name;

	json* content;
};

class Field {
public:
	char* name;

	json* content;
};

class Comment {
public:
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
	char* name;
	char* dataFormat;
	char* assetID;
	char* md5ext;
};

class Costume : public Asset {
public:
	int bitmapResolution;

	int rotationCenterX;
	int rotationCenterY;
};

class Sound : public Asset {
public:
	char* format;
	int rate;
	int sampleCount;
};