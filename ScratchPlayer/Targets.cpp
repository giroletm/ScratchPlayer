#include <iostream>
#include <string>
#include <ctime>
#include <chrono>

#include "Executor.h"


#define DISABLE_PRINTF

#ifdef DISABLE_PRINTF
#define printf(fmt, ...) (0)
#endif


Targets::Targets(json content) {
	data = content["targets"];

	int spriteCount = data.size();
	#ifdef _DEBUG
	printf("Found %d sprites.\n", spriteCount);
	#endif

	this->sprites.clear();
	for (int i = 0; i < spriteCount; i++) {
		#ifdef _DEBUG
		printf("Sprite %d:\n", i);
		#endif
		this->sprites.push_back(new Sprite(data[i]));
	}
}

Sprite::Sprite(json data) {
	this->isStage = data["isStage"];
	this->name = getCharsForJSON(data["name"]);

	this->currentCostume = data["currentCostume"];
	this->volume = data["volume"];
	this->layerOrder = data["layerOrder"];

	this->isClone = false;

	if (this->isStage) {
		this->tempo = data["tempo"];
		this->videoTransparency = data["videoTransparency"];
		this->videoState = getCharsForJSON(data["videoState"]);
		this->textToSpeechLanguage = getCharsForJSON(data["textToSpeechLanguage"]);

		this->visible = true;
		this->x = 0;
		this->y = 0;
		this->size = 100;
		this->direction = 90;
		this->draggable = false;
		this->rotationStyle = 0;
	}
	else {
		this->visible = data["visible"];
		this->x = data["x"];
		this->y = data["y"];
		this->size = data["size"];
		this->direction = data["direction"];
		this->draggable = data["draggable"];
		this->rotationStyle = getCharsForJSON(data["rotationStyle"]);

		this->tempo = 0;
		this->videoTransparency = 0;
		this->videoState = 0;
		this->textToSpeechLanguage = 0;
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
	for (int i = 0; i < blocksCount; i++) {
		if (this->blocks[i]->parent == 0) {
			this->blockSets.push_back(new BlockSet(this->blocks[i]));
		}
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

#pragma warning(disable:4996)
Sprite::Sprite(Sprite* original) {
	this->isStage = original->isStage;
	this->name = original->name;

	this->currentCostume = original->currentCostume;
	this->volume = original->volume;
	this->layerOrder = original->layerOrder;

	this->isClone = true;

	this->tempo = original->tempo;
	this->videoTransparency = original->videoTransparency;

	if (original->videoState) {
		this->videoState = new char[strlen(original->videoState) + 1];
		strcpy(this->videoState, original->videoState);
	}
	else this->videoState = 0;

	if (original->textToSpeechLanguage) {
		this->textToSpeechLanguage = new char[strlen(original->textToSpeechLanguage) + 1];
		strcpy(this->textToSpeechLanguage, original->textToSpeechLanguage);
	}
	else this->textToSpeechLanguage = 0;

	this->visible = original->visible;
	this->x = original->x;
	this->y = original->y;
	this->size = original->size;
	this->direction = original->direction;
	this->draggable = original->draggable;

	if (original->rotationStyle) {
		this->rotationStyle = new char[strlen(original->rotationStyle) + 1];
		strcpy(this->rotationStyle, original->rotationStyle);
	}
	else this->rotationStyle = 0;

	int varsCount = original->variables.size();
	int listsCount = original->lists.size();
	int broadcastsCount = original->broadcasts.size();
	int blocksCount = original->blocks.size();
	int blockSetsCount = original->blockSets.size();
	int commentsCount = original->comments.size();
	int costumesCount = original->costumes.size();
	int soundsCount = original->sounds.size();

	for (int i = 0; i < varsCount; i++) 
		this->variables.push_back(new Variable(original->variables[i]));

	for (int i = 0; i < listsCount; i++) 
		this->lists.push_back(original->lists[i]);

	for (int i = 0; i < broadcastsCount; i++) 
		this->broadcasts.push_back(original->broadcasts[i]);

	for (int i = 0; i < blocksCount; i++) 
		this->blocks.push_back(original->blocks[i]);

	for (int i = 0; i < blockSetsCount; i++) 
		this->blockSets.push_back(new BlockSet(original->blockSets[i]->firstBlock));

	for (int i = 0; i < commentsCount; i++)
		this->comments.push_back(original->comments[i]);

	for (int i = 0; i < costumesCount; i++) 
		this->costumes.push_back(original->costumes[i]);

	for (int i = 0; i < soundsCount; i++) 
		this->sounds.push_back(original->sounds[i]);
}

Variable::Variable(std::string id, json data) {
	this->uniqueID = getCharsForString(id);

	this->name = getCharsForJSON(data[0]);

	if (data[1].is_number()) {
		float v = data[1];
		this->value = getCharsForString(tostr(v));
	}
	else
		this->value = getCharsForJSON(data[1]);


	#ifdef _DEBUG
	printf("\t\t- Variable \"%s\" (\"%s\") = \"%s\".\n", this->name, this->uniqueID, this->value);
	#endif
}

Variable::Variable(Variable* variable) {
	this->uniqueID = getCharsForString(variable->uniqueID);
	this->name = getCharsForString(variable->name);
	this->value = getCharsForString(variable->value);
}

List::List(std::string id, json data) {
	this->uniqueID = getCharsForString(id);

	this->name = getCharsForJSON(data[0]);

	json valuesData = data[1];
	int valuesCount = valuesData.size();
	for (int i = 0; i < valuesCount; i++) {
		this->values.push_back(getCharsForJSON(valuesData[i]));
	}


	#ifdef _DEBUG
	printf("\t\t- List \"%s\" (\"%s\") = [", this->name, this->uniqueID);
	for (int i = 0; i < valuesCount; i++) {
		printf("\"%s\"%s", this->values[i], ((i == valuesCount - 1) ? "" : ", "));
	}
	printf("]\n");
	#endif
}

Broadcast::Broadcast(std::string id, json data) {
	this->uniqueID = getCharsForString(id);
	this->name = getCharsForJSON(data);


	#ifdef _DEBUG
	printf("\t\t- Broadcast \"%s\" (\"%s\").\n", this->name, this->uniqueID);
	#endif
}



static const char* motionCodes[] = {
	"movesteps",
	"turnright",
	"turnleft",
	"goto",
	"goto_menu",
	"gotoxy",
	"glideto",
	"glideto_menu",
	"glidesecstoxy",
	"pointindirection",
	"pointtowards",
	"pointtowards_menu",
	"changexby",
	"setx",
	"changeyby",
	"sety",
	"ifonedgebounce",
	"setrotationstyle",
	"xposition",
	"yposition",
	"direction"
};

static const char* looksCodes[] = {
	"sayforsecs",
	"say",
	"thinkforsecs",
	"think",
	"switchcostumeto",
	"costume",
	"nextcostume",
	"switchbackdropto",
	"backdrops",
	"nextbackdrop",
	"changesizeby",
	"setsizeto",
	"changeeffectby",
	"seteffectto",
	"cleargraphiceffects",
	"show",
	"hide",
	"gotofrontback",
	"goforwardbackwardlayers",
	"costumenumbername",
	"backdropnumbername",
	"size"
};

static const char* soundCodes[] = {
	"playuntildone",
	"sounds_menu",
	"play",
	"sounds_menu",
	"stopallsounds",
	"changeeffectby",
	"cleareffects",
	"changevolumeby",
	"setvolumeto",
	"volume"
};

static const char* eventCodes[] = {
	"whenflagclicked",
	"whenkeypressed",
	"whenthisspriteclicked",
	"whenbackdropswitchesto",
	"whengreaterthan",
	"whenbroadcastreceived",
	"broadcast",
	"broadcastandwait"
};

static const char* controlCodes[] = {
	"wait",
	"repeat",
	"forever",
	"if",
	"if_else",
	"wait_until",
	"repeat_until",
	"stop",
	"start_as_clone",
	"create_clone_of",
	"create_clone_of_menu",
	"delete_this_clone"
};

static const char* sensingCodes[] = {
	"touchingobject",
	"touchingobjectmenu",
	"touchingcolor",
	"coloristouchingcolor",
	"distanceto",
	"distancetomenu",
	"askandwait",
	"answer",
	"keypressed",
	"keyoptions",
	"mousedown",
	"mousex",
	"mousey",
	"setdragmode",
	"loudness",
	"timer",
	"resettimer",
	"of",
	"of_object_menu",
	"current",
	"dayssince2000",
	"username"
};

static const char* operatorCodes[] = {
	"add",
	"subtract",
	"multiply",
	"divide",
	"random",
	"gt",
	"lt",
	"equals",
	"and",
	"or",
	"not",
	"join",
	"letter_of",
	"length",
	"contains",
	"mod",
	"round",
	"mathop"
};

static const char* dataCodes[] = {
	"setvariableto",
	"changevariableby",
	"showvariable",
	"hidevariable",
	"addtolist",
	"deleteoflist",
	"deletealloflist",
	"insertatlist",
	"replaceitemoflist",
	"itemoflist",
	"itemnumoflist",
	"lengthoflist",
	"listcontainsitem",
	"showlist",
	"hidelist"
};

static const char* proceduresCodes[] = {
	"definition",
	"prototype",
	"call",
};

static const char* argumentCodes[] = {
	"reporter_string_number",
	"reporter_string_number",
	"reporter_boolean",
};

static const char* codesCats[]{
	"motion",
	"looks",
	"sound",
	"event",
	"control",
	"sensing",
	"operator",
	"data",
	"procedures",
	"argument",
};

static const char** codesVals[]{
	(const char**)&motionCodes,
	(const char**)&looksCodes,
	(const char**)&soundCodes,
	(const char**)&eventCodes,
	(const char**)&controlCodes,
	(const char**)&sensingCodes,
	(const char**)&operatorCodes,
	(const char**)&dataCodes,
	(const char**)&proceduresCodes,
	(const char**)&argumentCodes,
};

static Block::OpCode codesBegins[]{
	Block::OpCode::motion_movesteps,
	Block::OpCode::looks_sayforsecs,
	Block::OpCode::sound_playuntildone,
	Block::OpCode::event_whenflagclicked,
	Block::OpCode::control_wait,
	Block::OpCode::sensing_touchingobject,
	Block::OpCode::operator_add,
	Block::OpCode::data_setvariableto,
	Block::OpCode::procedures_definition,
	Block::OpCode::argument_reporter_string_number,
};

static const int blocksCount[]{
	(sizeof(motionCodes) / sizeof(const char*)),
	(sizeof(looksCodes) / sizeof(const char*)),
	(sizeof(soundCodes) / sizeof(const char*)),
	(sizeof(eventCodes) / sizeof(const char*)),
	(sizeof(controlCodes) / sizeof(const char*)),
	(sizeof(sensingCodes) / sizeof(const char*)),
	(sizeof(operatorCodes) / sizeof(const char*)),
	(sizeof(dataCodes) / sizeof(const char*)),
	(sizeof(proceduresCodes) / sizeof(const char*)),
	(sizeof(argumentCodes) / sizeof(const char*))
};

Block::OpCode Block::getOpCode(const char* opcode) {
	int catsCount = sizeof(codesCats) / sizeof(const char**);

	int num = 0;
	for (int i = 0; i < catsCount; i++) {
		int countLen = strlen(codesCats[i]);
		if (strncmp(opcode, codesCats[i], countLen) == 0) {
			const char** currentTbl = codesVals[i];
			int blockCount = blocksCount[i];

			for (int j = 0; j < blockCount; j++) {
				if (strcmp(currentTbl[j], opcode + (countLen + 1)) == 0) {
					num = codesBegins[i] + j;
					break;
				}
			}

			break;
		}
	}

	return (Block::OpCode)num;
}

Block::Block(std::string id, json data) {
	this->uniqueID = getCharsForString(id);

	if (data.is_array()) {
		this->opcode = OpCode::data_variable;
		this->x = data[3];
		this->y = data[4];

		this->inputs.push_back(new Input(data[2], data[1]));
	}
	else {
		std::string rawOpCode = data["opcode"];
		this->opcode = getOpCode(rawOpCode.c_str());

		this->shadow = data["shadow"];
		this->topLevel = data["topLevel"];

		if (data["parent"].is_null()) {
			this->x = data["x"];
			this->y = data["y"];
		}
		else {
			this->x = 0;
			this->y = 0;
		}

		this->parent = 0;
		this->next = 0;


		#ifdef _DEBUG
		printf("\t\t- Block \"%d\" (\"%s\").\n", this->opcode, this->uniqueID);
		#endif


		json inputsData = data["inputs"];
		for (auto i = inputsData.begin(); i != inputsData.end(); i++) {
			this->inputs.push_back(new Input(i.key(), i.value()));
		}


		json fieldsData = data["fields"];
		for (auto i = fieldsData.begin(); i != fieldsData.end(); i++) {
			this->fields.push_back(new Field(i.key(), i.value()));
		}

		// TODO: Custom blocks "mutation" element
	}
}

BlockSet::BlockSet(Block* block) {
	this->firstBlock = block;
	this->stackBlocks.clear();
	this->framesToWait = 0;
	this->scheduledFrameAction = -1;
	this->doneFlag = false;
	this->forceExecute = false;
	this->locked = false;

	this->framesToWait = 0;

	this->scheduledXTrans = 0.0f;
	this->scheduledXEnd = 0.0f;
	this->scheduledYTrans = 0.0f;
	this->scheduledYEnd = 0.0f;

	this->broadcastWait = 0;

	this->currentSubStack = 0;
	this->repeatTimes.clear();
	this->repeatBlock.clear();
}

Input::Input(std::string id, json data) {
	this->name = getCharsForString(id);
	this->content = data;


	#ifdef _DEBUG
	printf("\t\t\t- Input \"%s\" = %s.\n", this->name, data.dump().c_str());
	#endif
}

Field::Field(std::string id, json data) {
	this->name = getCharsForString(id);
	this->content = data;


	#ifdef _DEBUG
	printf("\t\t\t- Field \"%s\" = %s.\n", this->name, data.dump().c_str());
	#endif
}

Comment::Comment(std::string id, json data, Sprite* sprite) {
	this->uniqueID = getCharsForString(id);
	
	char* bID = getCharsForJSON(data["blockId"]);
	this->blockId = (bID) ? sprite->getBlockByUniqueID(bID) : 0;
	delete[] bID;

	this->x = data["x"];
	this->y = data["y"];
	this->width = data["width"];
	this->height = data["height"];
	this->minimized = data["minimized"];

	this->text = getCharsForJSON(data["text"]);


	#ifdef _DEBUG
	printf("\t\t- Comment \"%s\".\n", this->text);
	#endif
}

Asset::Asset(json data) {
	this->name = getCharsForJSON(data["name"]);
	this->dataFormat = getCharsForJSON(data["dataFormat"]);
	this->assetId = getCharsForJSON(data["assetId"]);
	this->md5ext = getCharsForJSON(data["md5ext"]);
}

Costume::Costume(json data) : Asset(data) {
	json bmpRes = data["bitmapResolution"];
	if (bmpRes.is_null()) this->bitmapResolution = 1;
	else this->bitmapResolution = bmpRes;

	this->rotationCenterX = data["rotationCenterX"];
	this->rotationCenterY = data["rotationCenterY"];

	u8* textureData = 0;
	size_t size = 0;
	Executor::instance->zfile->getContentForFile(this->md5ext, &textureData, &size, false);
	SDL_RWops *rw = SDL_RWFromMem(textureData, size);
	this->costumeTexture = IMG_LoadTexture_RW(Game::instance->renderer, rw, true);

	delete[] textureData;


	#ifdef _DEBUG
	printf("\t\t- Costume \"%s\".\n", this->name);
	#endif
}

Sound::Sound(json data) : Asset(data) {
	this->format = getCharsForJSON(data["format"]);
	this->rate = data["rate"];
	this->sampleCount = data["sampleCount"];


	#ifdef _DEBUG
	printf("\t\t- Sound \"%s\".\n", this->name);
	#endif
}


Targets::~Targets() {
	int spriteCount = this->sprites.size();
	for (int i = 0; i < spriteCount; i++)
		delete this->sprites[i];
}

Sprite::~Sprite() {
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
	int blockSetsCount = this->blockSets.size();

	for (int i = 0; i < varsCount; i++)
		delete this->variables[i];

	for (int i = 0; i < blockSetsCount; i++)
		delete this->blockSets[i];

	if (!this->isClone) {
		if (this->name)
			delete[] this->name;

		int listsCount = this->lists.size();
		int broadcastsCount = this->broadcasts.size();
		int blocksCount = this->blocks.size();
		int commentsCount = this->comments.size();
		int costumesCount = this->costumes.size();
		int soundsCount = this->sounds.size();

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
}

Variable::~Variable() {
	if (this->uniqueID)
		delete[] this->uniqueID;

	if (this->name)
		delete[] this->name;

	if (this->value)
		delete[] this->value;
}

List::~List() {
	if (this->uniqueID)
		delete[] this->uniqueID;

	if (this->name)
		delete[] this->name;

	int valuesount = this->values.size();
	for (int i = 0; i < valuesount; i++) {
		if (this->values[i])
			delete[] this->values[i];
	}
}

Broadcast::~Broadcast() {
	if (this->uniqueID)
		delete[] this->uniqueID;

	if (this->name)
		delete[] this->name;
}

Block::~Block() {
	if (this->uniqueID)
		delete[] this->uniqueID;

	int inputsCount = this->inputs.size();
	int fieldsCount = this->fields.size();

	for (int i = 0; i < inputsCount; i++)
		delete this->inputs[i];

	for (int i = 0; i < fieldsCount; i++)
		delete this->fields[i];
}

BlockSet::~BlockSet() {
}


Input::~Input() {
	if (this->name)
		delete[] this->name;
}

Field::~Field() {
	if (this->name)
		delete[] this->name;
}

Comment::~Comment() {
	if (this->uniqueID)
		delete[] this->uniqueID;

	if (this->text)
		delete[] this->text;
}

Asset::~Asset() {
	if (this->name)
		delete[] this->name;

	if (this->dataFormat)
		delete[] this->dataFormat;

	if (this->assetId)
		delete[] this->assetId;

	if (this->md5ext)
		delete[] this->md5ext;
}

Costume::~Costume() {
	if (this->costumeTexture)
		SDL_DestroyTexture(this->costumeTexture);
}

Sound::~Sound() {
	if (this->format)
		delete[] this->format;
}


Sprite* Targets::getSpriteByName(const char* name, Sprite* previous, bool noClone) {
	int spriteCount = sprites.size();
	if (!previous) {
		for (int i = 0; i < spriteCount; i++) {
			if (strcmp(this->sprites[i]->name, name) == 0) {
				if(!noClone || !this->sprites[i]->isClone) return this->sprites[i];
			}
		}
	}
	else {
		bool foundPrevious = false;
		for (int i = 0; i < spriteCount; i++) {
			if (!foundPrevious) {
				if (this->sprites[i] == previous) foundPrevious = true;
			}
			else if (strcmp(this->sprites[i]->name, name) == 0) 
				if (!noClone || !this->sprites[i]->isClone) return this->sprites[i];
		}
	}

	return 0;
}

Variable* Targets::getVariableByUniqueID(const char* uniqueID, int spriteID) {
	if (!uniqueID) return 0;

	Variable* bgVar = this->sprites[0]->getVariableByUniqueID(uniqueID);
	if (bgVar) return bgVar;
	else if(spriteID != 0) return this->sprites[spriteID]->getVariableByUniqueID(uniqueID);

	return 0;
}

Variable* Targets::getVariableByUniqueID(const char* uniqueID, Sprite* sprite) {
	if (!uniqueID) return 0;

	Variable* bgVar = this->sprites[0]->getVariableByUniqueID(uniqueID);
	if (bgVar) return bgVar;
	else if(sprite) return sprite->getVariableByUniqueID(uniqueID);

	return 0;
}

List* Targets::getListByUniqueID(const char* uniqueID, int spriteID) {
	if (!uniqueID) return 0;

	List* bgList = this->sprites[0]->getListByUniqueID(uniqueID);
	if (bgList) return bgList;
	else if (spriteID != 0) return this->sprites[spriteID]->getListByUniqueID(uniqueID);

	return 0;
}

Broadcast* Targets::getBroadcastByUniqueID(const char* uniqueID, int spriteID) {
	if (!uniqueID) return 0;

	Broadcast* bgBC = this->sprites[0]->getBroadcastByUniqueID(uniqueID);
	if (bgBC) return bgBC;
	else if (spriteID != 0) return this->sprites[spriteID]->getBroadcastByUniqueID(uniqueID);

	return 0;
}

void Targets::RemoveSprite(Sprite* sprite) {
	int spriteCount = sprites.size();
	for (int i = 0; i < spriteCount; i++) {
		if (sprites[i] == sprite) {
			sprites.erase(sprites.begin() + i);
			return;
		}
	}
}

void rotatePoints(float* pX, float* pY, float centerX, float centerY, float angle) { // https://stackoverflow.com/a/2259502
	float s = sin(angle);
	float c = cos(angle);

	// translate point back to origin:
	*pX -= centerX;
	*pY -= centerY;

	// rotate point
	float xnew = (*pX)* c - (*pY) * s;
	float ynew = (*pX)* s + (*pY)* c;

	// translate point back:
	*pX = xnew + centerX;
	*pY = ynew + centerY;
}

void Sprite::queryProps(float* xPtr, float* yPtr, float* wPtr, float* hPtr) {
	Costume* currentCostume = this->costumes[this->currentCostume];
	SDL_Texture* rTexture = currentCostume->costumeTexture;


	int wi, hi;
	SDL_QueryTexture(rTexture, NULL, NULL, &wi, &hi);
	float w = wi, h = hi;

	float sizeRatio = this->size / 100.0f;
	w *= sizeRatio;
	h *= sizeRatio;

	float cntX = currentCostume->rotationCenterX * sizeRatio;
	float cntY = currentCostume->rotationCenterY * sizeRatio;

	float x = this->x - cntX;
	float y = this->y + cntY;

	float TLx = x;
	float TLy = y;
	float TRx = x + w;
	float TRy = y;
	float BLx = x;
	float BLy = y - h;
	float BRx = x + w;
	float BRy = y - h;

	float cX = cntX + x;
	float cY = y - cntY;

	float angle = -(this->direction - 90.0f);

	if (angle != 0.0f) {
		rotatePoints(&TLx, &TLy, cX, cY, angle * (M_PI * 2) / 360.0f);
		rotatePoints(&TRx, &TRy, cX, cY, angle * (M_PI * 2) / 360.0f);
		rotatePoints(&BLx, &BLy, cX, cY, angle * (M_PI * 2) / 360.0f);
		rotatePoints(&BRx, &BRy, cX, cY, angle * (M_PI * 2) / 360.0f);
	}

	x = std::min(TLx, BLx);
	y = std::max(TLy, TRy);
	w = std::max(TRx, BRx) - x;
	h = y - std::min(BLy, BRy);

	//printf("xywh (%d;%d): %f %f %f %f\n", this->x, this->y, x, y, w, h);

	if (xPtr != NULL) *xPtr = x;
	if (yPtr != NULL) *yPtr = y;
	if (wPtr != NULL) *wPtr = w;
	if (hPtr != NULL) *hPtr = h;
}

void Sprite::queryProps(SDL_FPoint* TL, SDL_FPoint* TR, SDL_FPoint* BL, SDL_FPoint* BR) {
	Costume* currentCostume = this->costumes[this->currentCostume];
	SDL_Texture* rTexture = currentCostume->costumeTexture;


	int wi, hi;
	SDL_QueryTexture(rTexture, NULL, NULL, &wi, &hi);
	float w = wi, h = hi;

	float sizeRatio = this->size / 100.0f;
	w *= sizeRatio;
	h *= sizeRatio;

	float cntX = currentCostume->rotationCenterX * sizeRatio;
	float cntY = currentCostume->rotationCenterY * sizeRatio;

	float x = this->x - cntX;
	float y = this->y + cntY;

	float TLx = x;
	float TLy = y;
	float TRx = x + w;
	float TRy = y;
	float BLx = x;
	float BLy = y - h;
	float BRx = x + w;
	float BRy = y - h;

	float cX = cntX + x;
	float cY = y - cntY;

	float angle = -(this->direction - 90.0f);

	if(angle != 0.0f) {
		rotatePoints(&TLx, &TLy, cX, cY, angle * (M_PI * 2.0f) / 360.0f);
		rotatePoints(&TRx, &TRy, cX, cY, angle * (M_PI * 2.0f) / 360.0f);
		rotatePoints(&BLx, &BLy, cX, cY, angle * (M_PI * 2.0f) / 360.0f);
		rotatePoints(&BRx, &BRy, cX, cY, angle * (M_PI * 2.0f) / 360.0f);
	}

	if (TL != NULL) *TL = { TLx, TLy };
	if (TR != NULL) *TR = { TRx, TRy };
	if (BL != NULL) *BL = { BLx, BLy };
	if (BR != NULL) *BR = { BRx, BRy };
}

Variable* Sprite::getVariableByUniqueID(const char* uniqueID) {
	if (!uniqueID) return 0;

	int varsCount = this->variables.size();
	for (int i = 0; i < varsCount; i++) {
		if (strcmp(this->variables[i]->uniqueID, uniqueID) == 0) return this->variables[i];
	}

	return 0;
}

Variable* Sprite::getVariableByName(const char* name) {
	if (!name) return 0;

	int varsCount = this->variables.size();
	for (int i = 0; i < varsCount; i++) {
		if (strcmp(this->variables[i]->name, name) == 0) return this->variables[i];
	}

	return 0;
}

List* Sprite::getListByUniqueID(const char* uniqueID) {
	if (!uniqueID) return 0;

	int listsCount = this->lists.size();
	for (int i = 0; i < listsCount; i++) {
		if (strcmp(this->lists[i]->uniqueID, uniqueID) == 0) return this->lists[i];
	}

	return 0;
}

Broadcast* Sprite::getBroadcastByUniqueID(const char* uniqueID) {
	if (!uniqueID) return 0;

	int broadcastsCount = this->broadcasts.size();
	for (int i = 0; i < broadcastsCount; i++) {
		if (strcmp(this->broadcasts[i]->uniqueID, uniqueID) == 0) return this->broadcasts[i];
	}

	return 0;
}

Block* Sprite::getBlockByUniqueID(const char* uniqueID) {
	if (!uniqueID) return 0;

	int blockCount = this->blocks.size();
	for (int i = 0; i < blockCount; i++) {
		if (strcmp(this->blocks[i]->uniqueID, uniqueID) == 0) return this->blocks[i];
	}

	return 0;
}

Comment* Sprite::getCommentByUniqueID(const char* uniqueID) {
	int commentsCount = this->comments.size();
	for (int i = 0; i < commentsCount; i++) {
		if (strcmp(this->comments[i]->uniqueID, uniqueID) == 0) return this->comments[i];
	}

	return 0;
}

Costume* Sprite::getCostumeByName(const char* name) {
	int costumesCount = this->costumes.size();
	for (int i = 0; i < costumesCount; i++) {
		if (strcmp(this->costumes[i]->name, name) == 0) return this->costumes[i];
	}

	return 0;
}

int Sprite::getCostumeIDByName(const char* name) {
	int costumesCount = this->costumes.size();
	for (int i = 0; i < costumesCount; i++) {
		if (strcmp(this->costumes[i]->name, name) == 0) return i;
	}

	return -1;
}

Sound* Sprite::getSoundByName(const char* name) {
	int soundsCount = this->sounds.size();
	for (int i = 0; i < soundsCount; i++) {
		if (strcmp(this->sounds[i]->name, name) == 0) return this->sounds[i];
	}

	return 0;
}

Sprite* Sprite::Clone() {
	Sprite* clone = new Sprite(this);
	Executor::instance->targets->sprites.push_back(clone);

	return clone;
}


Input* Block::getInputByName(const char* name) {
	int inputCount = this->inputs.size();
	for (int i = 0; i < inputCount; i++) 
		if (strcmp(this->inputs[i]->name, name) == 0) return this->inputs[i];

	return 0;
}

Field* Block::getFieldByName(const char* name) {
	int fieldCount = this->fields.size();
	for (int i = 0; i < fieldCount; i++)
		if (strcmp(this->fields[i]->name, name) == 0) return this->fields[i];

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

float getLoudness() {
	return -1.0f; // Not implemented yet so consider no microphone
}

float getTimer() {
	return (float)Game::instance->getFrameCount() / 60.0f;
}

void resetTimer() {
	Game::instance->resetFrameCount();
}

std::string getGenericInputValue(Sprite* parentSprite, json content) {
	json cnt = content[1];
	if (!cnt.is_null()) {
		if (cnt.is_array()) {
			int type = cnt[0];
			if (type == 12) {
				char* varUID = getCharsForString(cnt[2]);
				std::string val = Executor::instance->targets->getVariableByUniqueID(varUID, parentSprite)->value;
				delete[] varUID;

				return val;
			}
			else if (type >= 4 && type <= 8) {
				std::string dist = cnt[1];
				return dist;
			}
			else if (type == 10) {
				char* text = getCharsForString(cnt[1]);
				Block* b = parentSprite->getBlockByUniqueID(text);
				std::string str = text;
				delete[] text;

				if (b) return b->getBlockValueAsString(parentSprite);
				else return str;
			}
		}
		else if (cnt.is_string()) {
			char* text = getCharsForString(cnt);
			Block* b = parentSprite->getBlockByUniqueID(text);
			std::string str = text;
			delete[] text;

			if (b) return b->getBlockValueAsString(parentSprite);
			else return str;
		}
	}
	else return "null";
}


std::string getGenericInputValueByName(Sprite* parentSprite, Block* currentBlock, const char* paramName) {
	return getGenericInputValue(parentSprite, currentBlock->getInputByName(paramName)->content);
}

std::string Block::getBlockValueAsString(Sprite* parentSprite) {
	std::string base = "";
	switch (this->opcode) {
		case OpCode::motion_xposition:
		{
			return tostr(parentSprite->x);
		}
		case OpCode::motion_yposition:
		{
			return tostr(parentSprite->y);
		}
		case OpCode::motion_direction:
		{
			return tostr(parentSprite->direction);
		}
		case OpCode::operator_add:
		case OpCode::operator_subtract:
		case OpCode::operator_multiply:
		case OpCode::operator_divide:
		case OpCode::operator_mod:
		{
			std::string operand1 = getGenericInputValue(parentSprite, this->getInputByName("NUM1")->content);
			std::string operand2 = getGenericInputValue(parentSprite, this->getInputByName("NUM2")->content);

			float op1 = str2float(operand1);
			float op2 = str2float(operand2);

			if(this->opcode == OpCode::operator_add)
				return tostr(op1 + op2);

			else if(this->opcode == OpCode::operator_subtract)
				return tostr(op1 - op2);

			else if(this->opcode == OpCode::operator_multiply)
				return tostr(op1 * op2);

			else if(this->opcode == OpCode::operator_divide)
				return tostr(op1 / op2);

			else //if(this->opcode == OpCode::operator_mod)
			{
				float res = op1;
				if (op2 > 0)
					while (res > op2) res -= op2;
				else if (op2 < 0)
					while (res < op2) res -= op2;
				else return "NaN";

				return tostr(res);
			}
		}
		case OpCode::operator_random:
		{
			std::string operand1 = getGenericInputValue(parentSprite, this->getInputByName("FROM")->content);
			std::string operand2 = getGenericInputValue(parentSprite, this->getInputByName("TO")->content);

			float op1 = str2float(operand1);
			float op2 = str2float(operand2);

			bool isInt = ((float)((int)(op1)) == op1) || ((float)((int)(op2)) == op2);

			float lim = (op2 - op1);
			if (!isInt)
				return tostr(((float)rand() / (float)(RAND_MAX / lim)) + op1);
			else
				return tostr((rand() % ((int)lim)) + (int)op1);
		}
		case OpCode::operator_gt:
		case OpCode::operator_lt:
		case OpCode::operator_equals:
		case OpCode::operator_and:
		case OpCode::operator_or:
		{
			std::string operand1 = getGenericInputValue(parentSprite, this->getInputByName("OPERAND1")->content);
			std::string operand2 = getGenericInputValue(parentSprite, this->getInputByName("OPERAND2")->content);

			float op1 = str2float(operand1);
			float op2 = str2float(operand2);
			int op1i = (int)op1;
			int op2i = (int)op2;

			bool result;

			if (this->opcode == OpCode::operator_gt)
				result = op1 > op2;

			else if (this->opcode == OpCode::operator_lt)
				result = op1 < op2;

			else if (this->opcode == OpCode::operator_equals)
				result = operand1 == operand2;

			else if (this->opcode == OpCode::operator_and)
				result = op1i && op2i;

			else //if (this->opcode == OpCode::operator_or)
				result = op1i || op2i;

			return result ? "true" : "false";
		}
		case OpCode::operator_not:
		{
			std::string operand = getGenericInputValue(parentSprite, this->getInputByName("OPERAND")->content);

			int op1i = (int)str2float(operand);

			return op1i ? "false" : "true";
		}
		case OpCode::operator_join:
		{
			std::string string1 = getGenericInputValue(parentSprite, this->getInputByName("STRING1")->content);
			std::string string2 = getGenericInputValue(parentSprite, this->getInputByName("STRING2")->content);

			return string1 + string2;
		}
		case OpCode::operator_letter_of:
		{
			std::string letter = getGenericInputValue(parentSprite, this->getInputByName("LETTER")->content);
			std::string string = getGenericInputValue(parentSprite, this->getInputByName("STRING")->content);

			int l = (int)str2float(letter) - 1;

			if (l < 0 || l >= string.size()) return "";

			return base + string[l];
		}
		case OpCode::operator_length:
		{
			std::string string = getGenericInputValue(parentSprite, this->getInputByName("STRING")->content);

			return tostr(string.size());
		}
		case OpCode::operator_contains:
		{
			std::string string1 = getGenericInputValue(parentSprite, this->getInputByName("STRING1")->content);
			std::string string2 = getGenericInputValue(parentSprite, this->getInputByName("STRING2")->content);

			return (string1.find(string2) != std::string::npos) ? "true" : "false";
		}
		case OpCode::operator_round:
		{
			std::string num = getGenericInputValue(parentSprite, this->getInputByName("NUM")->content);

			float n = str2float(num);

			return tostr(round(n));
		}
		case OpCode::operator_mathop:
		{
			std::string num = getGenericInputValue(parentSprite, this->getInputByName("NUM")->content);
			std::string op = this->getFieldByName("OPERATOR")->content[0];

			float n = str2float(num);
			float result = 0.0f;

			const char* opr = op.c_str();

			if (strcmp(opr, "abs") == 0) {
				result = abs(n);
			}
			else if (strcmp(opr, "floor") == 0) {
				result = floorf(n);
			}
			else if (strcmp(opr, "ceiling") == 0) {
				result = ceilf(n);
			}
			else if (strcmp(opr, "sqrt") == 0) {
				result = sqrtf(n);
			}
			else if (strcmp(opr, "sin") == 0) {
				result = sinf(n * (2.0f*M_PI) / 360.0f);
			}
			else if (strcmp(opr, "cos") == 0) {
				result = cosf(n * (2.0f * M_PI) / 360.0f);
			}
			else if (strcmp(opr, "tan") == 0) {
				result = tanf(n * (2.0f * M_PI) / 360.0f);
			}
			else if (strcmp(opr, "asin") == 0) {
				result = asinf(n) * 360.0f / (2.0f * M_PI);
			}
			else if (strcmp(opr, "acos") == 0) {
				result = acosf(n) * 360.0f / (2.0f * M_PI);
			}
			else if (strcmp(opr, "atan") == 0) {
				result = atanf(n) * 360.0f / (2.0f * M_PI);
			}
			else if (strcmp(opr, "ln") == 0) {
				result = logf(n);
			}
			else if (strcmp(opr, "log") == 0) {
				result = log10f(n);
			}
			else if (strcmp(opr, "e ^") == 0) {
				result = expf(n);
			}
			else if (strcmp(opr, "10 ^") == 0) {
				result = powf(10.0f, n);
			}

			return tostr(result);
		}
		case OpCode::looks_costumenumbername:
		{
			char* nn = getCharsForJSON(this->getFieldByName("NUMBER_NAME")->content[0]);
			
			std::string result = "";
			if (strcmp(nn, "number") == 0) result = tostr(parentSprite->currentCostume + 1);
			else if (strcmp(nn, "name") == 0) result = parentSprite->costumes[parentSprite->currentCostume]->name;

			delete[] nn;

			return result;
		}
		case OpCode::looks_backdropnumbername:
		{
			Sprite* bg = Executor::instance->targets->sprites[0];
			char* nn = getCharsForJSON(this->getFieldByName("NUMBER_NAME")->content[0]);

			std::string result = "";
			if (strcmp(nn, "number") == 0) result = tostr(bg->currentCostume + 1);
			else if (strcmp(nn, "name") == 0) result = bg->costumes[bg->currentCostume]->name;

			delete[] nn;

			return result;
		}
		case OpCode::looks_size:
		{
			return tostr(parentSprite->size);
		}
		case Block::OpCode::sensing_touchingobject:
		{
			char* tobjm = getCharsForJSON(this->getInputByName("TOUCHINGOBJECTMENU")->content[1]);
			if (!tobjm) break;
			Block* tobjmB = parentSprite->getBlockByUniqueID(tobjm);
			delete[] tobjm;

			char* touchingObj = getCharsForJSON(tobjmB->getFieldByName("TOUCHINGOBJECTMENU")->content[0]);
			if (!touchingObj) break;

			bool result = false;

			if (strcmp(touchingObj, "_mouse_") == 0) {
				MouseHandle* mh = &Game::instance->inputHandler.mouseHandle;

				float x, y, w, h;
				parentSprite->queryProps(&x, &y, &w, &h);

				result = collisionPoint(x, y, x+w, y-h, mh->x, mh->y);
			}
			else if (strcmp(touchingObj, "_edge_") == 0) {
				float x, y, w, h;
				parentSprite->queryProps(&x, &y, &w, &h);

				result = onEdge(x, y) || onEdge(x+w, y) || onEdge(x, y-h) || onEdge(x+w, y-h);
			}
			else {
				SDL_FPoint polyTbl1[4];
				parentSprite->queryProps(&(polyTbl1[0]), &(polyTbl1[1]), &(polyTbl1[3]), &(polyTbl1[2]));

				bool col = false;
				Sprite* destSprite = Executor::instance->targets->getSpriteByName(touchingObj);
				while (destSprite) {
					SDL_FPoint polyTbl2[4];
					destSprite->queryProps(&(polyTbl2[0]), &(polyTbl2[1]), &(polyTbl2[3]), &(polyTbl2[2]));

					result = collisionPolyPoly(polyTbl1, 4, polyTbl2, 4);
					if (result) break;

					destSprite = Executor::instance->targets->getSpriteByName(touchingObj, destSprite);
				}
			}

			delete[] touchingObj;

			return result ? "true" : "false";
		}
		// sensing_touchingobjectmenu is used by the sensing_touchingobject block, so it can't be attached directly to any block
		// sensing_touchingcolor & sensing_coloristouchingcolor will be implemented later if not never, because how am I even supposed to implement that?
		case Block::OpCode::sensing_distanceto:
		{
			char* distObjM = getCharsForJSON(this->getInputByName("DISTANCETOMENU")->content[1]);
			if (!distObjM) break;
			Block* distObjMB = parentSprite->getBlockByUniqueID(distObjM);
			delete[] distObjM;

			char* distanceObj = getCharsForJSON(distObjMB->getFieldByName("DISTANCETOMENU")->content[0]);
			if (!distanceObj) break;

			float result = 0.0f;
			if (strcmp(distanceObj, "_mouse_") == 0) {
				MouseHandle* mh = &Game::instance->inputHandler.mouseHandle;
				result = distance(parentSprite->x, parentSprite->y, mh->x, mh->y);
			}
			else {
				Sprite* destSprite = Executor::instance->targets->getSpriteByName(distanceObj, 0, true);
				result = distance(parentSprite->x, parentSprite->y, destSprite->x, destSprite->y);
			}

			delete[] distanceObj;

			return tostr(result);
		}
		// sensing_distancetomenu is used by the sensing_distanceto block, so it can't be attached directly to any block
		// sensing_answer will be implemented later
		case OpCode::sensing_keypressed:
		{
			char* keyOptM = getCharsForJSON(this->getInputByName("KEY_OPTION")->content[1]);
			if (!keyOptM) break;
			Block* keyOptMB = parentSprite->getBlockByUniqueID(keyOptM);
			delete[] keyOptM;

			char* keyOption = getCharsForJSON(keyOptMB->getFieldByName("KEY_OPTION")->content[0]);
			if (!keyOption) break;

			KeyHandle* kh = Game::instance->inputHandler.getKeyHandleByName(keyOption);
			delete[] keyOption;

			return kh->held ? "true" : "false";
		}
		// sensing_keyoptions is used by the sensing_keypressed block, so it can't be attached directly to any block
		case OpCode::sensing_mousedown:
		{
			bool left = Game::instance->inputHandler.getKeyHandle(SDL_BUTTON_LEFT)->held;
			bool right = Game::instance->inputHandler.getKeyHandle(SDL_BUTTON_RIGHT)->held;
			bool mid = Game::instance->inputHandler.getKeyHandle(SDL_BUTTON_MIDDLE)->held;
			return (left || right || mid) ? "true" : "false";
		}
		case OpCode::sensing_mousex:
		{
			return tostr(Game::instance->inputHandler.mouseHandle.x);
		}
		case OpCode::sensing_mousey:
		{
			return tostr(Game::instance->inputHandler.mouseHandle.y);
		}
		// sensing_setdragmode will be implemented later
		// sensing_loudness will probably not be implemented
		case OpCode::sensing_timer:
		{
			return tostr(getTimer());
		}
		case OpCode::sensing_of:
		{
			char* prop = getCharsForJSON(this->getFieldByName("PROPERTY")->content[0]);

			char* objM = getCharsForJSON(this->getInputByName("OBJECT")->content[1]);
			if (!objM) break;
			Block* objMB = parentSprite->getBlockByUniqueID(objM);
			delete[] objM;

			char* object = getCharsForJSON(objMB->getFieldByName("OBJECT")->content[0]);
			if (!object) break;

			Sprite* sprite = (strcmp(object, "_stage_") == 0) ? Executor::instance->targets->sprites[0] : Executor::instance->targets->getSpriteByName(object, 0, true);

			delete[] object;

			std::string toRet = "";
			if ((strcmp(prop, "backdrop #") == 0) || (strcmp(prop, "costume #") == 0)) {
				toRet = tostr(sprite->currentCostume + 1);
			}
			else if ((strcmp(prop, "backdrop name") == 0) || (strcmp(prop, "costume name") == 0)) {
				toRet = sprite->costumes[sprite->currentCostume]->name;
			}
			else if (strcmp(prop, "volume") == 0) {
				toRet = tostr(sprite->volume);
			}
			else if (strcmp(prop, "x position") == 0) {
				toRet = tostr(sprite->x);
			}
			else if (strcmp(prop, "y position") == 0) {
				toRet = tostr(sprite->y);
			}
			else if (strcmp(prop, "direction") == 0) {
				toRet = tostr(sprite->direction);
			}
			else if (strcmp(prop, "size") == 0) {
				toRet = tostr(sprite->size);
			}
			else {
				Variable* var = sprite->getVariableByName(prop);
				if (var) {
					toRet = var->value;
				}
			}

			delete[] prop;

			return toRet;
		}
		case OpCode::sensing_current:
		{
			char* currentMenu = getCharsForJSON(this->getFieldByName("CURRENTMENU")->content[0]);

			std::time_t t = std::time(0);
			std::tm* now = std::localtime(&t);

			int val = 0;
			if (strcmp(currentMenu, "YEAR") == 0) {
				val = now->tm_year + 1900;
			}
			else if (strcmp(currentMenu, "MONTH") == 0) {
				val = now->tm_mon;
			}
			else if (strcmp(currentMenu, "DATE") == 0) {
				val = now->tm_mday;
			}
			else if (strcmp(currentMenu, "DAYOFWEEK") == 0) {
				val = now->tm_wday + 1;
			}
			else if (strcmp(currentMenu, "HOUR") == 0) {
				val = now->tm_hour;
			}
			else if (strcmp(currentMenu, "MINUTE") == 0) {
				val = now->tm_min;
			}
			else if (strcmp(currentMenu, "SECOND") == 0) {
				val = now->tm_sec;
			}

			delete[] currentMenu;

			return tostr(val);
		}
		case OpCode::sensing_dayssince2000:
		{
			auto now = std::chrono::system_clock::now();

			double secs = std::chrono::duration_cast<std::chrono::seconds>(now.time_since_epoch()).count();

			secs -= 946684800.0f; // Subtracting number of seconds since 1970 and 2000 

			return tostr(secs / (60.0f * 60.0f * 24.0f));
		}
		case OpCode::sensing_username:
		{
			return "Human being";
		}
		case OpCode::data_variable:
		{
			return Executor::instance->targets->getVariableByUniqueID(this->inputs[0]->name, parentSprite)->value;
		}
	}

	return base;
}

void BlockSet::forceStop() {
	stackBlocks.clear();
	this->framesToWait = 0;
	this->scheduledFrameAction = -1;
	this->forceExecute = false;
	this->locked = false;

	this->framesToWait = 0;

	this->scheduledFrameAction = 0;
	this->scheduledXTrans = 0.0f;
	this->scheduledXEnd = 0.0f;
	this->scheduledYTrans = 0.0f;
	this->scheduledYEnd = 0.0f;

	this->broadcastWait = 0;

	currentSubStack = 0;
	repeatBlock.clear();
	repeatTimes.clear();
}

#include <math.h>

bool BlockSet::execute(Sprite* parentSprite) { // Returns true if the sprite needs to be skipped entirely after this blockset
	if (framesToWait) {
		framesToWait--;
		return false;
	}

	if (scheduledFrameAction > -1) {
		if (scheduledFrameAction) {
			parentSprite->x += scheduledXTrans;
			parentSprite->y += scheduledYTrans;
		}
		else {
			parentSprite->x = scheduledXEnd;
			parentSprite->y = scheduledYEnd;
		}

		scheduledFrameAction--;
		return false;
	}

	if (broadcastWait) {
		if (!Executor::instance->isBroadcastOn(broadcastWait)) broadcastWait = 0;
		else return false;
	}

	while (true) {
		if ((currentSubStack == 0) && (stackBlocks.size() == 0)) {
			bool triggered = false;

			switch (firstBlock->opcode) {
				case Block::OpCode::event_whenflagclicked:
				{
					if (!this->doneFlag && !parentSprite->isClone) {
						this->doneFlag = true;
						triggered = true;
					}
					break;
				}
				case Block::OpCode::event_whenkeypressed:
				{
					char* key = getCharsForJSON(firstBlock->getFieldByName("KEY_OPTION")->content[0]);
					if (key) {
						KeyHandle* kh = Game::instance->inputHandler.getKeyHandleByName(key);
						delete[] key;

						if (kh->pressed) triggered = true;
					}

					break;
				}
				case Block::OpCode::event_whenthisspriteclicked:
				{
					if (parentSprite->visible) {
						bool left = Game::instance->inputHandler.getKeyHandle(SDL_BUTTON_LEFT)->pressed;
						bool right = Game::instance->inputHandler.getKeyHandle(SDL_BUTTON_RIGHT)->pressed;
						bool mid = Game::instance->inputHandler.getKeyHandle(SDL_BUTTON_MIDDLE)->pressed;

						if (left || right || mid) {
							MouseHandle* mh = &Game::instance->inputHandler.mouseHandle;

							SDL_FPoint polyTbl[4];
							parentSprite->queryProps(&(polyTbl[0]), &(polyTbl[1]), &(polyTbl[3]), &(polyTbl[2]));

							//printf("ah: %f;%f  %f;%f  %f;%f  %f;%f  by %d;%d\n", polyTbl[0].x, polyTbl[0].y, polyTbl[1].x, polyTbl[1].y, polyTbl[2].x, polyTbl[2].y, polyTbl[3].x, polyTbl[3].y, mh->x, mh->y);
							/*
							int ww, wh;
							SDL_GetWindowSize(Game::instance->window, &ww, &wh);
							float wwpad = (ww / 2.0f);
							float whpad = (wh / 2.0f);
							
							SDL_RenderDrawLine(Game::instance->renderer, polyTbl[0].x + wwpad, -polyTbl[0].y + whpad, polyTbl[1].x + wwpad, -polyTbl[1].y + whpad);
							SDL_RenderDrawLine(Game::instance->renderer, polyTbl[1].x + wwpad, -polyTbl[1].y + whpad, polyTbl[2].x + wwpad, -polyTbl[2].y + whpad);
							SDL_RenderDrawLine(Game::instance->renderer, polyTbl[2].x + wwpad, -polyTbl[2].y + whpad, polyTbl[3].x + wwpad, -polyTbl[3].y + whpad);
							SDL_RenderDrawLine(Game::instance->renderer, polyTbl[3].x + wwpad, -polyTbl[3].y + whpad, polyTbl[0].x + wwpad, -polyTbl[0].y + whpad);
							*/
							/*
							for (int i = 0; i < 4; i++)
								Executor::instance->polyTbl[i] = polyTbl[i];
							*/

							triggered = collisionPolyPoint(&(polyTbl[0]), 4, mh->x, mh->y);
						}
					}

					break;
				}
				case Block::OpCode::event_whenbackdropswitchesto:
				{
					// Uses forceExecute
					break;
				}
				case Block::OpCode::event_whengreaterthan:
				{
					float value = str2float(getGenericInputValueByName(parentSprite, firstBlock, "VALUE"));
					char* varCmp = getCharsForJSON(firstBlock->getFieldByName("WHENGREATERTHANMENU")->content[0]);

					float val = 0.0f;
					if (strcmp(varCmp, "LOUDNESS") == 0) {
						val = getLoudness();
					}
					if (strcmp(varCmp, "TIMER") == 0) {
						val = getTimer();
					}

					if (!locked)
						triggered = val > value;

					locked = val > value;

					delete[] varCmp;

					break;
				}
				case Block::OpCode::event_whenbroadcastreceived:
				{
					// Uses forceExecute
					break;
				}
				case Block::OpCode::control_start_as_clone:
				{
					if (!this->doneFlag && parentSprite->isClone) {
						this->doneFlag = true;
						triggered = true;
					}
					break;
				}
			}

			if (forceExecute) {
				forceExecute = false;
				triggered = true;
			}

			//if (triggered) printf("Triggered!\n");
			//else printf("WaitForTrigger: %d (%d)\n", this->doneFlag, this->firstBlock->opcode);

			if (triggered) {
				stackBlocks.push_back(firstBlock->next);
				repeatBlock.push_back(firstBlock->next);
				repeatTimes.push_back(1);
			}
			else return false;
		}

		Block* currentBlock = stackBlocks[currentSubStack];
		bool halt = false;
		if (currentBlock) {
			bool switchSS = false;
			bool noNext = false;
			printf("Block %p, opcode %d -> ", currentBlock, currentBlock->opcode);
			switch (currentBlock->opcode) {
				case Block::OpCode::unsupported:
				{
					printf("unsupported\n");
					break;
				}
				case Block::OpCode::motion_movesteps:
				{
					float distance = str2float(getGenericInputValueByName(parentSprite, currentBlock, "STEPS"));

					parentSprite->x += distance * cos((parentSprite->direction - 90.0f) * M_PI / 180.0f);
					parentSprite->y -= distance * sin((parentSprite->direction - 90.0f) * M_PI / 180.0f);

					printf("Executing \"move %f steps\"\n", distance);

					break;
				}
				case Block::OpCode::motion_turnright:
				case Block::OpCode::motion_turnleft:
				{
					float degrees = str2float(getGenericInputValueByName(parentSprite, currentBlock, "DEGREES"));

					bool isLeft = (currentBlock->opcode == Block::OpCode::motion_turnleft);
					if (isLeft) degrees *= -1.0f;

					parentSprite->direction += degrees;

					printf("Executing \"turn %s %f degrees\"\n", isLeft ? "left" : "right", degrees);

					break;
				}
				case Block::OpCode::motion_goto:
				{
					char* dest = getCharsForJSON(currentBlock->getInputByName("TO")->content[1]);
					if (!dest) break;
					Block* destBlock = parentSprite->getBlockByUniqueID(dest);

					char* toGo = getCharsForJSON(destBlock->getFieldByName("TO")->content[0]);
					if (!toGo) break;

					float x = parentSprite->x;
					float y = parentSprite->y;
					if (strcmp(toGo, "_random_") == 0) {
						int ww, wh;
						SDL_GetWindowSize(Game::instance->window, &ww, &wh);

						x = (rand() % ww) - (ww / 2);;
						y = (rand() % wh) - (wh / 2);;
					}
					else if (strcmp(toGo, "_mouse_") == 0) {
						MouseHandle* mh = &Game::instance->inputHandler.mouseHandle;
						x = mh->x;
						y = mh->y;
					}
					else {
						Sprite* destSprite = Executor::instance->targets->getSpriteByName(toGo);
						if (destSprite) {
							x = destSprite->x;
							y = destSprite->y;
						}
					}

					parentSprite->x = x;
					parentSprite->y = y;

					printf("Executing \"go to %s\"\n", toGo);

					break;
				}
				// motion_goto_menu is used by the motion_goto block, so it can't be attached directly to any block
				case Block::OpCode::motion_gotoxy:
				{
					float paramX = str2float(getGenericInputValueByName(parentSprite, currentBlock, "X"));
					float paramY = str2float(getGenericInputValueByName(parentSprite, currentBlock, "Y"));

					printf("Executing \"go to x: %f  y: %f\"\n", paramX, paramY);

					parentSprite->x = paramX;
					parentSprite->y = paramY;

					break;
				}
				case Block::OpCode::motion_glideto:
				{
					char* dest = getCharsForJSON(currentBlock->getInputByName("TO")->content[1]);
					if (!dest) break;
					Block* destBlock = parentSprite->getBlockByUniqueID(dest);
					delete[] dest;

					char* toGo = getCharsForJSON(destBlock->getFieldByName("TO")->content[0]);
					if (!toGo) break;

					float paramSecs = str2float(getGenericInputValueByName(parentSprite, currentBlock, "SECS"));

					float x = parentSprite->x;
					float y = parentSprite->y;
					if (strcmp(toGo, "_random_") == 0) {
						int ww, wh;
						SDL_GetWindowSize(Game::instance->window, &ww, &wh);

						x = (rand() % ww) - (ww / 2);;
						y = (rand() % wh) - (wh / 2);;
					}
					else if (strcmp(toGo, "_mouse_") == 0) {

					}
					else {
						Sprite* destSprite = Executor::instance->targets->getSpriteByName(toGo);
						if (destSprite) {
							x = destSprite->x;
							y = destSprite->y;
						}
					}

					scheduledFrameAction = (int)round(paramSecs * 60.0f);
					scheduledXEnd = x;
					scheduledXTrans = (x - parentSprite->x) / (float)scheduledFrameAction;
					scheduledYEnd = y;
					scheduledYTrans = (y - parentSprite->y) / (float)scheduledFrameAction;

					printf("Executing \"glide %f secs to %s\"\n", paramSecs, toGo);

					delete[] toGo;

					halt = true;

					break;
				}
				// motion_glideto_menu is used by the motion_glideto block, so it can't be attached directly to any block
				case Block::OpCode::motion_glidesecstoxy:
				{
					float paramSecs = str2float(getGenericInputValueByName(parentSprite, currentBlock, "SECS"));
					float paramX = str2float(getGenericInputValueByName(parentSprite, currentBlock, "X"));
					float paramY = str2float(getGenericInputValueByName(parentSprite, currentBlock, "Y"));

					scheduledFrameAction = (int)round(paramSecs * 60.0f);
					scheduledXEnd = paramX;
					scheduledXTrans = (paramX - parentSprite->x) / (float)scheduledFrameAction;
					scheduledYEnd = paramY;
					scheduledYTrans = (paramY - parentSprite->y) / (float)scheduledFrameAction;

					printf("Executing \"glide %f secs to x: %f  y: %f\"\n", paramSecs, paramX, paramY);

					halt = true;

					break;
				}
				case Block::OpCode::motion_pointindirection:
				{
					float paramDir = str2float(getGenericInputValueByName(parentSprite, currentBlock, "DIRECTION"));

					parentSprite->direction = paramDir;

					printf("Executing \"point in direction %f\"\n", paramDir);

					break;
				}
				case Block::OpCode::motion_pointtowards:
				{
					char* dest = getCharsForJSON(currentBlock->getInputByName("TOWARDS")->content[1]);
					if (!dest) break;
					Block* destBlock = parentSprite->getBlockByUniqueID(dest);
					delete[] dest;

					char* toGo = getCharsForJSON(destBlock->getFieldByName("TOWARDS")->content[0]);
					if (!toGo) break;

					float x = parentSprite->x;
					float y = parentSprite->y;
					if (strcmp(toGo, "_mouse_") == 0) {

					}
					else {
						Sprite* destSprite = Executor::instance->targets->getSpriteByName(toGo);
						if (destSprite) {
							x = destSprite->x;
							y = destSprite->y;
						}
					}

					float delta_x = parentSprite->x - x;
					float delta_y = parentSprite->y - y;
					float theta_radians = atan2(delta_y, delta_x);

					parentSprite->direction = theta_radians * (360.0f / (2.0f * M_PI));

					printf("Executing \"point towards %s\"\n", toGo);

					delete[] toGo;

					break; 
				}
				// motion_pointtowards_menu is used by the motion_pointtowards block, so it can't be attached directly to any block
				case Block::OpCode::motion_changexby:
				{
					float paramX = str2float(getGenericInputValueByName(parentSprite, currentBlock, "DX"));

					parentSprite->x += paramX;

					printf("Executing \"change x by %f\"\n", paramX);

					break;
				}
				case Block::OpCode::motion_setx:
				{
					float paramX = str2float(getGenericInputValueByName(parentSprite, currentBlock, "X"));

					parentSprite->x = paramX;

					printf("Executing \"set x to %f\"\n", paramX);

					break;
				}
				case Block::OpCode::motion_changeyby:
				{
					float paramY = str2float(getGenericInputValueByName(parentSprite, currentBlock, "DY"));

					parentSprite->y += paramY;

					printf("Executing \"change y by %f\"\n", paramY);

					break;
				}
				case Block::OpCode::motion_sety:
				{
					float paramY = str2float(getGenericInputValueByName(parentSprite, currentBlock, "Y"));

					parentSprite->y = paramY;

					printf("Executing \"set y to %f\"\n", paramY);

					break;
				}
				case Block::OpCode::motion_ifonedgebounce:
				{
					printf("Executing \"if on edge, bounce\"\n");

					break;
				}
				case Block::OpCode::motion_setrotationstyle:
				{
					char* destRot = getCharsForJSON(currentBlock->getInputByName("STYLE")->content[0]);
					if (!destRot) break;

					delete[] parentSprite->rotationStyle;

					parentSprite->rotationStyle = destRot;

					printf("Executing \"set rotation style to %s\"\n", destRot);

					delete[] destRot;

					break;
				}
				// motion_xposition, motion_yposition and motion_direction are return-only
				case Block::OpCode::looks_sayforsecs:
				{
					std::string toSay = getGenericInputValueByName(parentSprite, currentBlock, "MESSAGE");
					float paramSecs = str2float(getGenericInputValueByName(parentSprite, currentBlock, "SECS"));

					framesToWait = (int)round(paramSecs * 60.0f);

					printf("Executing \"say %s for %f seconds\"\n", toSay.c_str(), paramSecs);

					halt = true;

					break;
				}
				case Block::OpCode::looks_say:
				{
					std::string toSay = getGenericInputValueByName(parentSprite, currentBlock, "MESSAGE");

					printf("Executing \"say %s\"\n", toSay.c_str());

					break;
				}
				case Block::OpCode::looks_thinkforsecs:
				{
					std::string toThink = getGenericInputValueByName(parentSprite, currentBlock, "MESSAGE");
					float paramSecs = str2float(getGenericInputValueByName(parentSprite, currentBlock, "SECS"));

					framesToWait = (int)round(paramSecs * 60.0f);

					printf("Executing \"think %s for %f seconds\"\n", toThink.c_str(), paramSecs);

					halt = true;

					break;
				}
				case Block::OpCode::looks_think:
				{
					std::string toThink = getGenericInputValueByName(parentSprite, currentBlock, "MESSAGE");

					printf("Executing \"think %s\"\n", toThink.c_str());

					break;
				}
				case Block::OpCode::looks_switchcostumeto:
				{
					char* cost = getCharsForJSON(currentBlock->getInputByName("COSTUME")->content[1]);
					if (!cost) break;
					Block* destBlock = parentSprite->getBlockByUniqueID(cost);
					delete[] cost;

					int i = -1;

					if (destBlock->opcode == Block::OpCode::looks_costume) {
						char* costume = getCharsForJSON(destBlock->getFieldByName("COSTUME")->content[0]);
						if (!costume) break;

						i = parentSprite->getCostumeIDByName(costume);
						delete[] costume;
					}
					else {
						i = (int)str2float(destBlock->getBlockValueAsString(parentSprite));
					}

					if (i < 0) break;

					parentSprite->currentCostume = i;

					printf("Executing \"switch costume to %s\"\n", parentSprite->costumes[i]->name);

					break;
				}
				// looks_costume is used by the looks_switchcostumeto block, so it can't be attached directly to any block
				case Block::OpCode::looks_nextcostume:
				{
					parentSprite->currentCostume++;
					if (parentSprite->currentCostume >= parentSprite->costumes.size()) parentSprite->currentCostume = 0;

					printf("Executing \"next costume\"\n");

					break;
				}
				case Block::OpCode::looks_switchbackdropto:
				{
					char* cost = getCharsForJSON(currentBlock->getInputByName("BACKDROP")->content[1]);
					if (!cost) break;
					Block* destBlock = parentSprite->getBlockByUniqueID(cost);
					delete[] cost;

					Sprite* bg = Executor::instance->targets->sprites[0];
					int i = -1;

					if (destBlock->opcode == Block::OpCode::looks_backdrops) {
						char* backdrop = getCharsForJSON(destBlock->getFieldByName("BACKDROP")->content[0]);
						if (!backdrop) break;

						i = bg->getCostumeIDByName(backdrop);
						delete[] backdrop;
					}
					else {
						i = (int)str2float(destBlock->getBlockValueAsString(parentSprite));
					}

					if (i < 0) break;

					bg->currentCostume = i;

					Executor::instance->triggerBackdropSwitch(bg->costumes[i]->name);

					printf("Executing \"switch backdrop to %s\"\n", bg->costumes[i]->name);


					break;
				}
				// looks_backdrops is used by the looks_switchbackdropto block, so it can't be attached directly to any block
				case Block::OpCode::looks_nextbackdrop:
				{
					Sprite* bg = Executor::instance->targets->sprites[0];

					bg->currentCostume++;
					if (bg->currentCostume >= bg->costumes.size()) bg->currentCostume = 0;

					printf("Executing \"next backdrop\"\n");

					Executor::instance->triggerBackdropSwitch(bg->costumes[bg->currentCostume]->name);

					break;
				}
				case Block::OpCode::looks_changesizeby:
				{
					float paramSize = str2float(getGenericInputValueByName(parentSprite, currentBlock, "CHANGE"));

					parentSprite->size += paramSize;
					if (parentSprite->size < 5.0f) parentSprite->size = 5.0f;

					printf("Executing \"change size by %f\"\n", paramSize);

					break;
				}
				case Block::OpCode::looks_setsizeto:
				{
					float paramSize = str2float(getGenericInputValueByName(parentSprite, currentBlock, "SIZE"));

					parentSprite->size = paramSize;
					if (parentSprite->size < 5.0f) parentSprite->size = 5.0f;

					printf("Executing \"set size to %f\"\n", paramSize);

					break;
				}
				// looks_changeeffectby, looks_seteffectto & looks_cleargraphiceffects will be implemented later (or won't be imlemented at all)
				case Block::OpCode::looks_show:
				{
					parentSprite->visible = true;

					printf("Executing \"show\"\n");

					break;
				}
				case Block::OpCode::looks_hide:
				{
					parentSprite->visible = false;

					printf("Executing \"show\"\n");

					break;
				}
				case Block::OpCode::looks_gotofrontback:
				{
					char* frontback = getCharsForJSON(currentBlock->getFieldByName("FRONT_BACK")->content[0]);

					Targets* targets = Executor::instance->targets;
					int spriteCount = targets->data.size();


					if (strcmp(frontback, "front")) {
						int maxLayer = INT_MIN;
						for (int i = 0; i < spriteCount; i++) {
							Sprite* currentSprite = targets->sprites[i];
							if (currentSprite->layerOrder > maxLayer) maxLayer = currentSprite->layerOrder;
						}

						parentSprite->layerOrder = maxLayer;
					}
					else if (strcmp(frontback, "back")) {
						int minLayer = INT_MAX;
						for (int i = 0; i < spriteCount; i++) {
							Sprite* currentSprite = targets->sprites[i];
							if (currentSprite->layerOrder < minLayer) minLayer = currentSprite->layerOrder;
						}

						parentSprite->layerOrder = minLayer;
					}

					printf("Executing \"go to %s layer\"\n", frontback);

					delete[] frontback;

					break;
				}
				case Block::OpCode::looks_goforwardbackwardlayers:
				{
					int paramNum = (int)str2float(getGenericInputValueByName(parentSprite, currentBlock, "NUM"));
					char* forwback = getCharsForJSON(currentBlock->getFieldByName("FORWARD_BACKWARD")->content[0]);


					if (strcmp(forwback, "front"))
						parentSprite->layerOrder += paramNum;
					else if (strcmp(forwback, "backward"))
						parentSprite->layerOrder -= paramNum;

					printf("Executing \"go %s %d layer\"\n", forwback, paramNum);

					delete[] forwback;

					break;
				}
				// looks_costumenumbername, looks_backdropnumbername and looks_size are return-only
				case Block::OpCode::event_broadcast:
				{
					char* brName = getCharsForJSON(currentBlock->getInputByName("BROADCAST_INPUT")->content[1][1]);
					char* brID = getCharsForJSON(currentBlock->getInputByName("BROADCAST_INPUT")->content[1][2]);

					Executor::instance->triggerBroadcast(brID);

					printf("Executing \"broadcast %s\"\n", brName);

					delete[] brName;
					delete[] brID;

					break;
				}
				case Block::OpCode::event_broadcastandwait:
				{
					char* brName = getCharsForJSON(currentBlock->getInputByName("BROADCAST_INPUT")->content[1][1]);
					char* brID = getCharsForJSON(currentBlock->getInputByName("BROADCAST_INPUT")->content[1][2]);

					Executor::instance->triggerBroadcast(brID);

					printf("Executing \"broadcast %s and wait\"\n", brName);

					broadcastWait = brID;

					delete[] brName;

					break;
				}
				case Block::OpCode::control_wait:
				{
					float paramDura = str2float(getGenericInputValueByName(parentSprite, currentBlock, "DURATION"));

					framesToWait = (int)round(paramDura * 60.0f);

					printf("Executing \"wait %f seconds\"\n", paramDura);

					halt = true;

					break;
				}
				case Block::OpCode::control_repeat:
				{
					int paramTimes = (int)str2float(getGenericInputValueByName(parentSprite, currentBlock, "TIMES"));
					Input* ipt = currentBlock->getInputByName("SUBSTACK");
					if (!ipt) break;

					char* ss = getCharsForJSON(ipt->content[1]);
					Block* b = parentSprite->getBlockByUniqueID(ss);
					delete[] ss;
					if (!b) break;

					repeatTimes.push_back(paramTimes);
					stackBlocks.push_back(b);
					repeatBlock.push_back(b);

					stackBlocks[currentSubStack] = currentBlock->next;

					switchSS = true;

					printf("Executing \"repeat %d time (%s)\"\n", paramTimes, b->uniqueID);


					break;
				}
				case Block::OpCode::control_forever:
				{
					Input* ipt = currentBlock->getInputByName("SUBSTACK");
					if (!ipt) break;

					char* ss = getCharsForJSON(ipt->content[1]);
					Block* b = parentSprite->getBlockByUniqueID(ss);
					if (!b) break;

					repeatTimes.push_back(-1);
					stackBlocks.push_back(b);
					repeatBlock.push_back(b);

					stackBlocks[currentSubStack] = currentBlock->next;

					switchSS = true;

					printf("Executing \"repeat forever (%s)\"\n", ss);

					delete[] ss;

					break;
				}
				case Block::OpCode::control_if:
				{
					Input* ipt = currentBlock->getInputByName("SUBSTACK");
					if (!ipt) break;

					Input* cnd = currentBlock->getInputByName("CONDITION");
					if (!cnd) break;

					char* ss = getCharsForJSON(ipt->content[1]);
					Block* b = parentSprite->getBlockByUniqueID(ss);
					delete[] ss;
					if (!b) break;

					bool doCond = (int)str2float(getGenericInputValue(parentSprite, cnd->content));

					if (doCond && b) {
						repeatTimes.push_back(1);
						stackBlocks.push_back(b);
						repeatBlock.push_back(b);

						stackBlocks[currentSubStack] = currentBlock->next;

						switchSS = true;
					}

					printf("Executing \"if %d then %s\"\n", doCond, b ? b->uniqueID : "0");

					break;
				}
				case Block::OpCode::control_if_else:
				{
					Input* ipt = currentBlock->getInputByName("SUBSTACK");
					Input* ipt2 = currentBlock->getInputByName("SUBSTACK2");

					if (!ipt && !ipt2) break;

					Input* cnd = currentBlock->getInputByName("CONDITION");

					Block* b = 0;
					Block* b2 = 0;
					if (ipt) {
						char* ss = getCharsForJSON(ipt->content[1]);
						b = parentSprite->getBlockByUniqueID(ss);
						delete[] ss;
					}
					if (ipt2) {
						char* ss2 = getCharsForJSON(ipt2->content[1]);
						b2 = parentSprite->getBlockByUniqueID(ss2);
						delete[] ss2;
					}


					bool doCond = (cnd) ? (int)str2float(getGenericInputValue(parentSprite, cnd->content)) : 0;

					//repeatNext = currentBlock->next;
					//currentBlock = 0;

					if (doCond) {
						if (b) {
							repeatTimes.push_back(1);
							stackBlocks.push_back(b);
							repeatBlock.push_back(b);

							stackBlocks[currentSubStack] = currentBlock->next;

							switchSS = true;
						}
					}
					else {
						if (b2) {
							repeatTimes.push_back(1);
							stackBlocks.push_back(b2);
							repeatBlock.push_back(b2);

							stackBlocks[currentSubStack] = currentBlock->next;

							switchSS = true;
						}
					}

					printf("Executing \"if %d then %s else %s\"\n", doCond, b ? b->uniqueID : "0", b2 ? b2->uniqueID : "0");

					break;
				}
				case Block::OpCode::control_wait_until:
				{
					Input* cnd = currentBlock->getInputByName("CONDITION");
					if (!cnd) {
						noNext = true;
						break;
					}

					char* ss = getCharsForJSON(cnd->content[1]);
					Block *b = parentSprite->getBlockByUniqueID(ss);
					delete[] ss;

					int val = (b) ? (int)str2float(b->getBlockValueAsString(parentSprite)) : 0;
					if (!val) {
						noNext = true;
						halt = true;
					}

					printf("Executing \"wait until %s (=%d)\"\n", b->uniqueID, val);

					break;
				}
				case Block::OpCode::control_repeat_until:
				{
					Input* cnd = currentBlock->getInputByName("CONDITION");
					Input* ipt = currentBlock->getInputByName("SUBSTACK");
					if (!cnd) {
						noNext = true;
						break;
					}

					char* ss = getCharsForJSON(cnd->content[1]);
					Block *b = parentSprite->getBlockByUniqueID(ss);
					delete[] ss;

					int val = (b) ? (int)str2float(b->getBlockValueAsString(parentSprite)) : 0;
					if (!val) {
						halt = true;

						Input* ipt = currentBlock->getInputByName("SUBSTACK");
						if (!ipt) break;

						char* ss = getCharsForJSON(ipt->content[1]);
						Block* b2 = parentSprite->getBlockByUniqueID(ss);
						delete[] ss;
						if (!b) break;

						repeatTimes.push_back(1);
						stackBlocks.push_back(b2);
						repeatBlock.push_back(b2);

						switchSS = true;
					}

					printf("Executing \"repeat until %s (=%d)\"\n", b->uniqueID, val);

					break;
				}
				case Block::OpCode::control_stop:
				{
					std::string opt = currentBlock->getFieldByName("STOP_OPTION")->content[0];

					bool ret = false;
					if (strcmp(opt.c_str(), "other scripts in sprite") == 0) {
						Executor::instance->stopBlockSets(this, parentSprite);
					}
					else if (strcmp(opt.c_str(), "all") == 0) {
						Executor::instance->stopBlockSets();
						ret = true;
					}
					else if (strcmp(opt.c_str(), "this script") == 0) {
						this->forceStop();
						ret = true;
					}

					printf("Executing \"stop %s\"\n", opt.c_str());
					if (ret) return false;

					break;
				}
				case Block::OpCode::control_create_clone_of:
				{
					char* toClone = getCharsForJSON(currentBlock->getInputByName("CLONE_OPTION")->content[1]);
					if (!toClone) break;
					Block* toCloneBlock = parentSprite->getBlockByUniqueID(toClone);
					delete[] toClone;

					char* cloning = getCharsForJSON(toCloneBlock->getFieldByName("CLONE_OPTION")->content[0]);
					if (!cloning) break;

					if (strcmp(cloning, "_myself_") == 0) {
						parentSprite->Clone();
					}
					else {
						Sprite* destSprite = Executor::instance->targets->getSpriteByName(cloning);
						destSprite->Clone();
					}

					printf("Executing \"create clone of %s\"\n", cloning);

					delete[] cloning;

					break;
				}
				// control_create_clone_of_menu is used by the control_create_clone_of block, so it can't be attached directly to any block
				case Block::OpCode::control_delete_this_clone:
				{
					bool ret = false;
					if (parentSprite->isClone) {
						Executor::instance->targets->RemoveSprite(parentSprite);

						delete parentSprite;

						this->forceStop();
						ret = true;
					}

					printf("Executing \"delete this clone\"\n");

					if (ret) return true;
					break;
				}
				case Block::OpCode::sensing_askandwait:
				{
					std::string q = getGenericInputValue(parentSprite, currentBlock->getInputByName("QUESTION")->content);

					framesToWait = -1; // To implement

					printf("Executing \"ask %s and wait\"\n", q.c_str());

					break;
				}
				case Block::OpCode::sensing_resettimer:
				{
					resetTimer();

					printf("Executing \"reset timer\"\n");

					break;
				}
				case Block::OpCode::data_setvariableto:
				{
					std::string paramValue = getGenericInputValueByName(parentSprite, currentBlock, "VALUE");

					char* vID = getCharsForJSON(currentBlock->getFieldByName("VARIABLE")->content[1]);
					Variable* var = Executor::instance->targets->getVariableByUniqueID(vID, parentSprite);
					delete[] vID;
					if (var) {
						delete[] var->value;
						var->value = getCharsForString(paramValue);
					}

					printf("Executing \"set %s to %s\"\n", var->name, paramValue.c_str());

					break;
				}
				case Block::OpCode::data_changevariableby:
				{
					float paramValue = str2float(getGenericInputValueByName(parentSprite, currentBlock, "VALUE"));

					char* vID = getCharsForJSON(currentBlock->getFieldByName("VARIABLE")->content[1]);
					Variable* var = Executor::instance->targets->getVariableByUniqueID(vID, parentSprite);
					if (var) {
						float origValue = str2float(var->value);
						delete[] var->value;
						var->value = getCharsForString(tostr(origValue + paramValue));
					}

					printf("Executing \"change %s by %f\"\n", var->name, paramValue);

					break;
				}
			}

			if (!noNext) {
				if (!switchSS) {
					if (currentBlock->next) {
						//printf("Switching to next block\n");
						stackBlocks[currentSubStack] = currentBlock->next;
					}
					else {
						while (true) {
							//printf("No next block: ");
							if (currentSubStack == 0) {
								//printf("Finished script\n");
								stackBlocks.clear();
								repeatBlock.clear();
								repeatTimes.clear();

								break;
							}
							else if (repeatTimes[currentSubStack] == 0) {
								//printf("Back one stack\n");
								stackBlocks.pop_back();
								repeatBlock.pop_back();
								repeatTimes.pop_back();

								currentSubStack--;
								currentBlock = stackBlocks[currentSubStack];
								stackBlocks[currentSubStack] = currentBlock;

								if (currentBlock != 0) break;

							}
							else {
								//printf("Got a repeat to do (%d) at %p (Stack %d)\n", repeatTimes[currentSubStack], repeatBlock[currentSubStack], currentSubStack);
								stackBlocks[currentSubStack] = repeatBlock[currentSubStack];
								if(repeatTimes[currentSubStack] > 0)
									repeatTimes[currentSubStack]--;

								break;
							}
						}

						break;
					}
				}
				else {
					//printf("Forward one stack\n");
					currentSubStack++;
					repeatTimes[currentSubStack]--;
					currentBlock = stackBlocks[currentSubStack];
				}
			}

			if (halt) break;
		}
		else break;
	}

	return false;
}