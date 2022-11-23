#pragma once

#include <vector>;
#include <string>

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

class BlockSet;

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

	// Not in the actual structure, used for execution
	std::vector<BlockSet*> blockSets;


	Variable* getVariableByUniqueID(const char* uniqueID);
	List* getListByUniqueID(const char* uniqueID);
	Broadcast* getBroadcastByUniqueID(const char* uniqueID);
	Block* getBlockByUniqueID(const char* uniqueID);
	Comment* getCommentByUniqueID(const char* uniqueID);

	Costume* getCostumeByName(const char* name);
	int getCostumeIDByName(const char* name);
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

	enum OpCode {
		unsupported = 0,

		// Motion
		motion_movesteps = 1,
		motion_turnright = 2,
		motion_turnleft = 3,
		motion_goto = 4,
		motion_goto_menu = 5,
		motion_gotoxy = 6,
		motion_glideto = 7,
		motion_glideto_menu = 8,
		motion_glidesecstoxy = 9,
		motion_pointindirection = 10,
		motion_pointtowards = 11,
		motion_pointtowards_menu = 12,
		motion_changexby = 13,
		motion_setx = 14,
		motion_changeyby = 15,
		motion_sety = 16,
		motion_ifonedgebounce = 17,
		motion_setrotationstyle = 18,
		motion_xposition = 19,
		motion_yposition = 20,
		motion_direction = 21,

		// Looks
		looks_sayforsecs = 22,
		looks_say = 23,
		looks_thinkforsecs = 24,
		looks_think = 25,
		looks_switchcostumeto = 26,
		looks_costume = 27,
		looks_nextcostume = 28,
		looks_switchbackdropto = 29,
		looks_backdrops = 30,
		looks_nextbackdrop = 31,
		looks_changesizeby = 32,
		looks_setsizeto = 33,
		looks_changeeffectby = 34,
		looks_seteffectto = 35,
		looks_cleargraphiceffects = 36,
		looks_show = 37,
		looks_hide = 38,
		looks_gotofrontback = 39,
		looks_goforwardbackwardlayers = 40,
		looks_costumenumbername = 41,
		looks_backdropnumbername = 42,
		looks_size = 43,

		// Sound
		sound_playuntildone = 44,
		sound_sounds_menu = 45,
		sound_play = 46,
		sound_stopallsounds = 47,
		sound_changeeffectby = 48,
		sound_cleareffects = 49,
		sound_changevolumeby = 50,
		sound_setvolumeto = 51,
		sound_volume = 52,

		// Events
		event_whenflagclicked = 53,
		event_whenkeypressed = 54,
		event_whenthisspriteclicked = 55,
		event_whenbackdropswitchesto = 56,
		event_whengreaterthan = 57,
		event_whenbroadcastreceived = 58,
		event_broadcast = 59,
		event_broadcastandwait = 60,

		// Control
		control_wait = 61,
		control_repeat = 62,
		control_forever = 63,
		control_if = 64,
		control_if_else = 65,
		control_wait_until = 66,
		control_repeat_until = 67,
		control_stop = 68,
		control_start_as_clone = 69,
		control_create_clone_of = 70,
		control_create_clone_of_menu = 71,
		control_delete_this_clone = 72,


		// Sensing
		sensing_touchingobject = 73,
		sensing_touchingobjectmenu = 74,
		sensing_touchingcolor = 75,
		sensing_coloristouchingcolor = 76,
		sensing_distanceto = 77,
		sensing_distancetomenu = 78,
		sensing_askandwait = 79,
		sensing_answer = 80,
		sensing_keypressed = 81,
		sensing_keyoptions = 82,
		sensing_mousedown = 83,
		sensing_mousex = 84,
		sensing_mousey = 85,
		sensing_setdragmode = 86,
		sensing_loudness = 87,
		sensing_timer = 88,
		sensing_resettimer = 89,
		sensing_of = 90,
		sensing_of_object_menu = 91,
		sensing_current = 92,
		sensing_dayssince2000 = 93,
		sensing_username = 94,

		// Operators
		operator_add = 95,
		operator_subtract = 96,
		operator_multiply = 97,
		operator_divide = 98,
		operator_random = 99,
		operator_gt = 100,
		operator_lt = 101,
		operator_equals = 102,
		operator_and = 103,
		operator_or = 104,
		operator_not = 105,
		operator_join = 106,
		operator_letter_of = 107,
		operator_length = 108,
		operator_contains = 109,
		operator_mod = 110,
		operator_round = 111,
		operator_mathop = 112,

		// Variables
		/*Alone variable block*/
		data_setvariableto = 113,
		data_changevariableby = 114,
		data_showvariable = 115,
		data_hidevariable = 116,
		data_addtolist = 117,
		data_deleteoflist = 118,
		data_deletealloflist = 119,
		data_insertatlist = 120,
		data_replaceitemoflist = 121,
		data_itemoflist = 122,
		data_itemnumoflist = 123,
		data_lengthoflist = 124,
		data_listcontainsitem = 125,
		data_showlist = 126,
		data_hidelist = 127,

		// My Blocks
		procedures_definition = 128,
		procedures_prototype = 129,
		procedures_call = 130,
		argument_reporter_string_number = 131,
		argument_reporter_boolean = 132,
	};

	char* uniqueID;

	OpCode opcode;

	Block* next;
	Block* parent;

	std::vector<Input*> inputs;
	std::vector<Field*> fields;

	bool shadow;
	bool topLevel;

	int x;
	int y;


	OpCode getOpCode(const char* opcode);
	void doParenting(Sprite* sprite, json data);

	Input* getInputByName(const char* name);
	Field* getFieldByName(const char* name);

	std::string getBlockValueAsString(Sprite* parentSprite);
};

class BlockSet {
public:
	BlockSet(Block* block);
	~BlockSet();

	Block* firstBlock;
	Block* currentBlock;

	bool doneFlag;

	int framesToWait;

	int scheduledFrameAction;
	float scheduledXTrans;
	float scheduledXEnd;
	float scheduledYTrans;
	float scheduledYEnd;

	void execute(Sprite* parentSprite);
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