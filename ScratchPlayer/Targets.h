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

	// Not in the actual structure, used for execution
	std::vector<BlockSet*> blockSets;


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

	enum OpCode {
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
		sound_sounds_menu = 47,
		sound_stopallsounds = 48,
		sound_changeeffectby = 49,
		sound_cleareffects = 50,
		sound_changevolumeby = 51,
		sound_setvolumeto = 52,
		sound_volume = 53,

		// Events
		event_whenflagclicked = 54,
		event_whenkeypressed = 55,
		event_whenthisspriteclicked = 56,
		event_whenbackdropswitchesto = 57,
		event_whengreaterthan = 58,
		event_whenbroadcastreceived = 59,
		event_broadcast = 60,
		event_broadcastandwait = 61,

		// Control
		control_wait = 62,
		control_repeat = 63,
		control_forever = 64,
		control_if = 65,
		control_if_else = 66,
		control_wait_until = 67,
		control_repeat_until = 68,
		control_stop = 69,
		control_start_as_clone = 70,
		control_create_clone_of = 71,
		control_create_clone_of_menu = 72,
		control_delete_this_clone = 73,


		// Sensing
		sensing_touchingobject = 74,
		sensing_touchingobjectmenu = 75,
		sensing_touchingcolor = 76,
		sensing_coloristouchingcolor = 77,
		sensing_distanceto = 78,
		sensing_distancetomenu = 79,
		sensing_askandwait = 80,
		sensing_answer = 81,
		sensing_keypressed = 82,
		sensing_keyoptions = 83,
		sensing_mousedown = 84,
		sensing_mousex = 85,
		sensing_mousey = 86,
		sensing_setdragmode = 87,
		sensing_loudness = 88,
		sensing_timer = 89,
		sensing_resettimer = 90,
		sensing_of = 91,
		sensing_of_object_menu = 92,
		sensing_current = 93,
		sensing_dayssince2000 = 94,
		sensing_username = 95,

		// Operators
		operator_add = 96,
		operator_subtract = 97,
		operator_multiply = 98,
		operator_divide = 99,
		operator_random = 100,
		operator_gt = 101,
		operator_lt = 102,
		operator_equals = 103,
		operator_and = 104,
		operator_or = 105,
		operator_not = 106,
		operator_join = 107,
		operator_letter_of = 108,
		operator_length = 109,
		operator_contains = 110,
		operator_mod = 111,
		operator_round = 112,
		operator_mathop = 113,

		// Variables
		/*Alone variable block*/
		data_setvariableto = 114,
		data_changevariableby = 115,
		data_showvariable = 116,
		data_hidevariable = 117,
		data_addtolist = 118,
		data_deleteoflist = 119,
		data_deletealloflist = 120,
		data_insertatlist = 121,
		data_replaceitemoflist = 122,
		data_itemoflist = 123,
		data_itemnumoflist = 124,
		data_lengthoflist = 125,
		data_listcontainsitem = 126,
		data_showlist = 127,
		data_hidelist = 128,

		// My Blocks
		procedures_definition = 129,
		procedures_prototype = 130,
		argument_reporter_string_number = 131,
		argument_reporter_string_number = 132,
		argument_reporter_boolean = 133,
		procedures_call = 134,
	};

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

class BlockSet {
public:
	BlockSet(Block* block);
	~BlockSet();

	Block* firstBlock;
	Block* currentBlock;

	int framesToWait;
	int scheduledFrameAction;
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