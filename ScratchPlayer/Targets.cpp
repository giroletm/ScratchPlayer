#include <iostream>
#include <string>

#include "Executor.h"


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

		this->visible = true;
		this->x = 0;
		this->y = 0;
		this->size = 100;
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

Variable::Variable(std::string id, json data) {
	this->uniqueID = getCharsForString(id);

	this->name = getCharsForJSON(data[0]);
	this->value = getCharsForJSON(data[1]);


	#ifdef _DEBUG
	printf("\t\t- Variable \"%s\" (\"%s\") = \"%s\".\n", this->name, this->uniqueID, this->value);
	#endif
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
	(sizeof(soundCodes) / sizeof(const char*)),
	(sizeof(soundCodes) / sizeof(const char*)),
	(sizeof(eventCodes) / sizeof(const char*)),
	(sizeof(controlCodes) / sizeof(const char*)),
	(sizeof(sensingCodes) / sizeof(const char*)),
	(sizeof(operatorCodes) / sizeof(const char*)),
	(sizeof(dataCodes) / sizeof(const char*)),
	(sizeof(proceduresCodes) / sizeof(const char*)),
	(sizeof(argumentCodes) / sizeof(const char*))
};

Block::OpCode Block::getOpCode(char* opcode) {
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

	char* rawOpCode = getCharsForJSON(data["opcode"]);
	this->opcode = getOpCode(rawOpCode);;
	delete[] rawOpCode;
	
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

BlockSet::BlockSet(Block* block) {
	this->firstBlock = block;
	this->currentBlock = 0;
	this->framesToWait = 0;
	this->scheduledFrameAction = -1;
	this->doneFlag = false;
}

Input::Input(std::string id, json data) {
	this->name = getCharsForString(id);
	this->content = data;


	#ifdef _DEBUG
	char* val = getCharsForString(data.dump());
	printf("\t\t\t- Input \"%s\" = %s.\n", this->name, val);
	delete[] val;
	#endif
}

Field::Field(std::string id, json data) {
	this->name = getCharsForString(id);
	this->content = data;


	#ifdef _DEBUG
	char* val = getCharsForString(data.dump());
	printf("\t\t\t- Field \"%s\" = %s.\n", this->name, val);
	delete[] val;
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
	if (this->sprites) {
		int spriteCount = data.size();
		for (int i = 0; i < spriteCount; i++) {
			delete this->sprites[i];
		}

		delete[] this->sprites;
	}
}

Sprite::~Sprite() {
	if(this->name)
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
	int blockSetsCount = this->blockSets.size();
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

	for (int i = 0; i < blockSetsCount; i++)
		delete this->blocks[i];

	for (int i = 0; i < blocksCount; i++)
		delete this->blockSets[i];

	for (int i = 0; i < commentsCount; i++)
		delete this->comments[i];

	for (int i = 0; i < costumesCount; i++)
		delete this->costumes[i];

	for (int i = 0; i < soundsCount; i++)
		delete this->sounds[i];
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


Sprite* Targets::getSpriteByName(const char* name) {
	int spriteCount = data.size();
	for (int i = 0; i < spriteCount; i++) {
		if (strcmp(this->sprites[i]->name, name) == 0) return this->sprites[i];
	}

	return 0;
}

Variable* Targets::getVariableByUniqueID(const char* uniqueID, int spriteID) {
	Variable* bgVar = this->sprites[0]->getVariableByUniqueID(uniqueID);
	if (bgVar) return bgVar;
	else if(spriteID != 0) return this->sprites[spriteID]->getVariableByUniqueID(uniqueID);

	return 0;
}

List* Targets::getListByUniqueID(const char* uniqueID, int spriteID) {
	List* bgList = this->sprites[0]->getListByUniqueID(uniqueID);
	if (bgList) return bgList;
	else if (spriteID != 0) return this->sprites[spriteID]->getListByUniqueID(uniqueID);

	return 0;
}

Broadcast* Targets::getBroadcastByUniqueID(const char* uniqueID, int spriteID) {
	Broadcast* bgBC = this->sprites[0]->getBroadcastByUniqueID(uniqueID);
	if (bgBC) return bgBC;
	else if (spriteID != 0) return this->sprites[spriteID]->getBroadcastByUniqueID(uniqueID);

	return 0;
}



Variable* Sprite::getVariableByUniqueID(const char* uniqueID) {
	int varsCount = this->variables.size();
	for (int i = 0; i < varsCount; i++) {
		if (strcmp(this->variables[i]->uniqueID, uniqueID) == 0) return this->variables[i];
	}

	return 0;
}

List* Sprite::getListByUniqueID(const char* uniqueID) {
	int listsCount = this->lists.size();
	for (int i = 0; i < listsCount; i++) {
		if (strcmp(this->lists[i]->uniqueID, uniqueID) == 0) return this->lists[i];
	}

	return 0;
}

Broadcast* Sprite::getBroadcastByUniqueID(const char* uniqueID) {
	int broadcastsCount = this->broadcasts.size();
	for (int i = 0; i < broadcastsCount; i++) {
		if (strcmp(this->broadcasts[i]->uniqueID, uniqueID) == 0) return this->broadcasts[i];
	}

	return 0;
}

Block* Sprite::getBlockByUniqueID(const char* uniqueID) {
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

Sound* Sprite::getSoundByName(const char* name) {
	int soundsCount = this->sounds.size();
	for (int i = 0; i < soundsCount; i++) {
		if (strcmp(this->sounds[i]->name, name) == 0) return this->sounds[i];
	}

	return 0;
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

#include <sstream>

template <typename T>
std::string tostr(const T& t) {
	std::ostringstream os;
	os << t;
	return os.str();
}

std::string Block::getBlockValueAsString(Sprite* parentSprite) {
	std::string base = "";
	switch (this->opcode) {
		case OpCode::motion_xposition:
		{
			return tostr(round(parentSprite->x));
		}
		case OpCode::motion_yposition:
		{
			return tostr(parentSprite->y);
		}
		case OpCode::motion_direction:
		{
			return tostr(parentSprite->direction);
		}
	}

	return base;
}

std::string getGenericInputValue(Sprite* parentSprite, json content) {
	if (content[0] == 3) {
		char* bID = getCharsForJSON(content[1]);
		std::string dist = parentSprite->getBlockByUniqueID(bID)->getBlockValueAsString(parentSprite);
		delete[] bID;

		return dist;
	}
	else {
		std::string dist = content[1][1];
		return dist;
	}
}


std::string getGenericInputValueByName(Sprite* parentSprite, Block* currentBlock, const char* paramName) {
	return getGenericInputValue(parentSprite, currentBlock->getInputByName(paramName)->content);
}

#include <math.h>

void BlockSet::execute(Sprite* parentSprite) {
	if (framesToWait) {
		framesToWait--;
		return;
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
		return;
	}

	while (true) {
		if (currentBlock == 0) {
			bool triggered = false;

			switch (firstBlock->opcode) {
				case Block::OpCode::event_whenflagclicked:
				{
					if (!this->doneFlag) {
						this->doneFlag = true;
						triggered = true;
					}
					break;
				}
				case Block::OpCode::event_whenkeypressed:
				{
					char* key = getCharsForJSON(firstBlock->getFieldByName("KEY_OPTION")->content[0]);
					KeyHandle* kh = Game::instance->inputHandler.getKeyHandleByName(key);
					delete[] key;

					if (kh->pressed) triggered = true;

					break;
				}
				case Block::OpCode::event_whenthisspriteclicked:
				{
					break;
				}
				case Block::OpCode::event_whenbackdropswitchesto:
				{
					break;
				}
				case Block::OpCode::event_whengreaterthan:
				{
					break;
				}
				case Block::OpCode::event_whenbroadcastreceived:
				{
					break;
				}
				case Block::OpCode::control_start_as_clone:
				{
					break;
				}
			}

			//if (triggered) printf("Triggered!\n");
			//else printf("WaitForTrigger: %d (%d)\n", this->doneFlag, this->firstBlock->opcode);

			if (triggered) currentBlock = firstBlock->next;
			else return;
		}

		bool halt = false;
		if (currentBlock) {
			printf("Block %p, opcode %d -> ", currentBlock, currentBlock->opcode);
			switch (currentBlock->opcode) {
				case Block::OpCode::motion_movesteps:
				{
					float distance = std::stof(getGenericInputValueByName(parentSprite, currentBlock, "STEPS"));

					parentSprite->x += distance * cos((parentSprite->direction - 90.0f) * M_PI / 180.0f);
					parentSprite->y -= distance * sin((parentSprite->direction - 90.0f) * M_PI / 180.0f);

					printf("Executing \"move %f steps\"\n", distance);

					break;
				}
				case Block::OpCode::motion_turnright:
				case Block::OpCode::motion_turnleft:
				{
					float degrees = std::stof(getGenericInputValueByName(parentSprite, currentBlock, "DEGREES"));

					bool isLeft = (currentBlock->opcode == Block::OpCode::motion_turnleft);
					if (isLeft) degrees *= -1.0f;

					parentSprite->direction += degrees;

					printf("Executing \"turn %s %f degrees\"\n", isLeft ? "left" : "right", degrees);

					break;
				}
				case Block::OpCode::motion_goto:
				{
					char* dest = getCharsForJSON(currentBlock->getInputByName("TO")->content[1]);
					Block* destBlock = parentSprite->getBlockByUniqueID(dest);
					delete[] dest;

					char* toGo = getCharsForJSON(destBlock->getFieldByName("TO")->content[0]);

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

					parentSprite->x = x;
					parentSprite->y = y;

					printf("Executing \"go to %s\"\n", toGo);

					delete[] toGo;

					break;
				}
				// motion_goto_menu is used by the motion_goto block, so it can't be attatched directly to any block
				case Block::OpCode::motion_gotoxy:
				{
					float paramX = std::stof(getGenericInputValueByName(parentSprite, currentBlock, "X"));
					float paramY = std::stof(getGenericInputValueByName(parentSprite, currentBlock, "Y"));

					printf("Executing \"go to x: %f  y: %f\"\n", paramX, paramY);

					parentSprite->x = paramX;
					parentSprite->y = paramY;

					break;
				}
				case Block::OpCode::motion_glideto:
				{
					char* dest = getCharsForJSON(currentBlock->getInputByName("TO")->content[1]);
					Block* destBlock = parentSprite->getBlockByUniqueID(dest);
					delete[] dest;

					char* toGo = getCharsForJSON(destBlock->getFieldByName("TO")->content[0]);

					float paramSecs = std::stof(getGenericInputValueByName(parentSprite, currentBlock, "SECS"));

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

					break;
				}
				// motion_glideto_menu is used by the motion_glideto block, so it can't be attatched directly to any block
				case Block::OpCode::motion_glidesecstoxy:
				{
					float paramSecs = std::stof(getGenericInputValueByName(parentSprite, currentBlock, "SECS"));
					float paramX = std::stof(getGenericInputValueByName(parentSprite, currentBlock, "X"));
					float paramY = std::stof(getGenericInputValueByName(parentSprite, currentBlock, "Y"));

					scheduledFrameAction = (int)round(paramSecs * 60.0f);
					scheduledXEnd = paramX;
					scheduledXTrans = (paramX - parentSprite->x) / (float)scheduledFrameAction;
					scheduledYEnd = paramY;
					scheduledYTrans = (paramY - parentSprite->y) / (float)scheduledFrameAction;

					printf("Executing \"glide %f secs to x: %f  y: %f\"\n", paramSecs, paramX, paramY);

					break;
				}
				case Block::OpCode::motion_pointindirection:
				{
					float paramDir = std::stof(getGenericInputValueByName(parentSprite, currentBlock, "DIRECTION"));

					parentSprite->direction = paramDir;

					printf("Executing \"point in direction %f\"\n", paramDir);

					break;
				}
				case Block::OpCode::motion_pointtowards:
				{
					char* dest = getCharsForJSON(currentBlock->getInputByName("TOWARDS")->content[1]);
					Block* destBlock = parentSprite->getBlockByUniqueID(dest);
					delete[] dest;

					char* toGo = getCharsForJSON(destBlock->getFieldByName("TOWARDS")->content[0]);

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
				// motion_pointtowards_menu is used by the motion_pointtowards block, so it can't be attatched directly to any block
				case Block::OpCode::motion_changexby:
				{
					float paramX = std::stof(getGenericInputValueByName(parentSprite, currentBlock, "DX"));

					parentSprite->x += paramX;

					printf("Executing \"change x by %f\"\n", paramX);

					break;
				}
				case Block::OpCode::motion_setx:
				{
					float paramX = std::stof(getGenericInputValueByName(parentSprite, currentBlock, "X"));

					parentSprite->x = paramX;

					printf("Executing \"set x to %f\"\n", paramX);

					break;
				}
				case Block::OpCode::motion_changeyby:
				{
					float paramY = std::stof(getGenericInputValueByName(parentSprite, currentBlock, "DY"));

					parentSprite->y += paramY;

					printf("Executing \"change y by %f\"\n", paramY);

					break;
				}
				case Block::OpCode::motion_sety:
				{
					float paramY = std::stof(getGenericInputValueByName(parentSprite, currentBlock, "Y"));

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

					delete[] parentSprite->rotationStyle;

					parentSprite->rotationStyle = destRot;

					printf("Executing \"set rotation style to %s\"\n", destRot);

					break;
				}
				// motion_xposition, motion_yposition and motion_direction are return-only
				case Block::OpCode::looks_sayforsecs:
				{
					std::string toSay = getGenericInputValueByName(parentSprite, currentBlock, "MESSAGE");
					float paramSecs = std::stof(getGenericInputValueByName(parentSprite, currentBlock, "SECS"));

					char* ts = getCharsForString(toSay);
					printf("Executing \"say %s for %f seconds\"\n", ts, paramSecs);
					delete[] ts;
				}
			}

			if (currentBlock->next) currentBlock = currentBlock->next;
			else {
				currentBlock = 0;
				break;
			}

			if (halt) break;
		}
	}
}