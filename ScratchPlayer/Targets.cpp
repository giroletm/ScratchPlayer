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

Block::Block(std::string id, json data) {
	this->uniqueID = getCharsForString(id);

	this->opcode = getCharsForJSON(data["opcode"]);
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
	printf("\t\t- Block \"%s\" (\"%s\").\n", this->opcode, this->uniqueID);
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

	if (this->opcode)
		delete[] this->opcode;

	int inputsCount = this->inputs.size();
	int fieldsCount = this->fields.size();

	for (int i = 0; i < inputsCount; i++)
		delete this->inputs[i];

	for (int i = 0; i < fieldsCount; i++)
		delete this->fields[i];
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