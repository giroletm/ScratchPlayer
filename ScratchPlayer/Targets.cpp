#include <iostream>
#include <string>

#include "Executor.h"


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

BlockSet::BlockSet(Block* block) {
	this->firstBlock = block;
	this->stackBlocks.clear();
	this->framesToWait = 0;
	this->scheduledFrameAction = -1;
	this->doneFlag = false;
	this->forceExecute = false;
	this->locked = false;

	this->broadcastWait = 0;

	this->currentSubStack = 0;
	this->repeatTimes.clear();
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
	if (!uniqueID) return 0;

	Variable* bgVar = this->sprites[0]->getVariableByUniqueID(uniqueID);
	if (bgVar) return bgVar;
	else if(spriteID != 0) return this->sprites[spriteID]->getVariableByUniqueID(uniqueID);

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



Variable* Sprite::getVariableByUniqueID(const char* uniqueID) {
	if (!uniqueID) return 0;

	int varsCount = this->variables.size();
	for (int i = 0; i < varsCount; i++) {
		if (strcmp(this->variables[i]->uniqueID, uniqueID) == 0) return this->variables[i];
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

std::string getGenericInputValue(Sprite* parentSprite, json content) {
	if (content[0] & 2) {
		char* bID = getCharsForString(content[1]);
		std::string dist = parentSprite->getBlockByUniqueID(bID)->getBlockValueAsString(parentSprite);
		delete[] bID;

		return dist;
	}
	else {
		json cnt = content[1];
		if (!cnt.is_null()) {
			std::string dist = cnt[1];
			return dist;
		}
		else return "null";
	}
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
				result = op1 == op2;

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
	}

	return base;
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

	if (broadcastWait) {
		if (!Executor::instance->isBroadcastOn(broadcastWait)) broadcastWait = 0;
		else return;
	}

	while (true) {
		if ((currentSubStack == 0) && (stackBlocks.size() == 0)) {
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
					if (key) {
						KeyHandle* kh = Game::instance->inputHandler.getKeyHandleByName(key);
						delete[] key;

						if (kh->pressed) triggered = true;
					}

					break;
				}
				case Block::OpCode::event_whenthisspriteclicked:
				{
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
					// Uses forceExecute
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
			else return;
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

					char* costume = getCharsForJSON(destBlock->getFieldByName("COSTUME")->content[0]);
					if (!costume) break;

					int i = parentSprite->getCostumeIDByName(costume);
					if (i < 0) break;

					parentSprite->currentCostume = i;

					printf("Executing \"switch costume to %s\"\n", costume);

					delete[] costume;

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

					char* backdrop = getCharsForJSON(destBlock->getFieldByName("BACKDROP")->content[0]);
					if (!backdrop) break;

					Sprite* bg = Executor::instance->targets->sprites[0];
					int i = bg->getCostumeIDByName(backdrop);
					if (i < 0) break;

					bg->currentCostume = i;

					Executor::instance->triggerBackdropSwitch(backdrop);

					printf("Executing \"switch backdrop to %s\"\n", backdrop);

					delete[] backdrop;

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
								//printf("Got a repeat to do (%d) at %p\n", repeatTimes[currentSubStack], repeatBlock[currentSubStack]);
								stackBlocks[currentSubStack] = repeatBlock[currentSubStack];
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
	}
}