#include "Executor.h"

Executor* Executor::instance = 0;

Executor::Executor(const char* sb3Filename) {
    instance = this;

    this->zfile = new ZipFile(sb3Filename);

    u8* content = 0;;
    size_t size;
    int error = zfile->getContentForFile("project.json", &content, &size, true);

    if (error == 0) {
        this->targets = new Targets(json::parse((char*)content));
    }
    else this->targets = 0;

    if (content)
        delete[] content;

    this->isRunning = false;
    this->hangOn = IMG_LoadTexture(Game::instance->renderer, "assets/HangOn.png");
}

Executor::~Executor() {
    delete targets;
    delete zfile;

    SDL_DestroyTexture(this->hangOn);

    instance = 0;
}

void Executor::executeBlocks() {
    if (!isRunning) return;

    // Duplicating to prevent sprite removal problems
    std::vector<Sprite*> spriteList = targets->sprites;
    int spriteCount = spriteList.size();

    for (int spriteID = 0; spriteID < spriteCount; spriteID++) {
        Sprite* currentSprite = spriteList[spriteID];

        int blockSetCount = currentSprite->blockSets.size();
        for (int bSID = 0; bSID < blockSetCount; bSID++) {
            BlockSet* currentBS = currentSprite->blockSets[bSID];

            if (currentBS->execute(currentSprite)) break;
        }
    }
}

void Executor::stopBlockSets(BlockSet* bs, Sprite* s) {
    if (bs && s) {
        int blockSetCount = s->blockSets.size();
        for (int bSID = 0; bSID < blockSetCount; bSID++) {
            BlockSet* currentBS = s->blockSets[bSID];

            if(currentBS != bs) currentBS->forceStop();
        }
    }
    else {
        int spriteCount = targets->sprites.size();
        for (int spriteID = 0; spriteID < spriteCount; spriteID++) {
            Sprite* currentSprite = targets->sprites[spriteID];

            int blockSetCount = currentSprite->blockSets.size();
            for (int bSID = 0; bSID < blockSetCount; bSID++) {
                BlockSet* currentBS = currentSprite->blockSets[bSID];

                currentBS->forceStop();
            }
        }
    }
}

void Executor::triggerBackdropSwitch(const char* name) {
    int spriteCount = targets->sprites.size();
    for (int spriteID = 0; spriteID < spriteCount; spriteID++) {
        Sprite* currentSprite = targets->sprites[spriteID];

        int blockSetCount = currentSprite->blockSets.size();
        for (int bSID = 0; bSID < blockSetCount; bSID++) {
            BlockSet* currentBS = currentSprite->blockSets[bSID];

            if (currentBS->firstBlock->opcode == Block::OpCode::event_whenbackdropswitchesto) {
                char* varCmp = getCharsForJSON(currentBS->firstBlock->getFieldByName("BACKDROP")->content[0]);

                if (strcmp(varCmp, name) == 0) currentBS->forceExecute = true;

                delete[] varCmp;
            }
        }
    }
}

void Executor::triggerBroadcast(const char* uniqueID) {
    int spriteCount = targets->sprites.size();
    for (int spriteID = 0; spriteID < spriteCount; spriteID++) {
        Sprite* currentSprite = targets->sprites[spriteID];

        int blockSetCount = currentSprite->blockSets.size();
        for (int bSID = 0; bSID < blockSetCount; bSID++) {
            BlockSet* currentBS = currentSprite->blockSets[bSID];

            if (currentBS->firstBlock->opcode == Block::OpCode::event_whenbroadcastreceived) {
                char* varCmp = getCharsForJSON(currentBS->firstBlock->getFieldByName("BROADCAST_OPTION")->content[1]);

                if (strcmp(varCmp, uniqueID) == 0) currentBS->forceExecute = true;

                delete[] varCmp;
            }
        }
    }
}

bool Executor::isBroadcastOn(const char* uniqueID) {
    int spriteCount = targets->sprites.size();
    for (int spriteID = 0; spriteID < spriteCount; spriteID++) {
        Sprite* currentSprite = targets->sprites[spriteID];

        int blockSetCount = currentSprite->blockSets.size();
        for (int bSID = 0; bSID < blockSetCount; bSID++) {
            BlockSet* currentBS = currentSprite->blockSets[bSID];

            if ((currentBS->firstBlock->opcode == Block::OpCode::event_whenbroadcastreceived) && currentBS->stackBlocks.size() != 0) {
                char* varCmp = getCharsForJSON(currentBS->firstBlock->getFieldByName("BROADCAST_OPTION")->content[1]);

                bool corresponds = (strcmp(varCmp, uniqueID) == 0);
                delete[] varCmp;

                if (corresponds) return true;
            }
        }
    }

    return false;
}

void Executor::render() {
    int spriteCount = targets->sprites.size();
    int minLayer = INT_MAX;
    int maxLayer = INT_MIN;
    for (int i = 0; i < spriteCount; i++) {
        Sprite* currentSprite = targets->sprites[i];
        if (currentSprite->layerOrder < minLayer) minLayer = currentSprite->layerOrder;
        if (currentSprite->layerOrder > maxLayer) maxLayer = currentSprite->layerOrder;
    }

    for (int layer = minLayer; layer <= maxLayer; layer++) {
        for (int i = 0; i < spriteCount; i++) {
            Sprite* currentSprite = targets->sprites[i];
            if (currentSprite->layerOrder == layer && currentSprite->visible) {
                Costume* currentCostume = currentSprite->costumes[currentSprite->currentCostume];
                SDL_Texture* rTexture = currentCostume->costumeTexture;

                int ww, wh;
                SDL_GetWindowSize(Game::instance->window, &ww, &wh);

                if (i == 0) {
                    SDL_Rect dRect = { 0, 0, ww, wh };
                    SDL_RenderCopy(Game::instance->renderer, rTexture, NULL, &dRect);

                    //printf("Rendering Sprite %d (Background)\n", i);
                }
                else {
                    int wi, hi;
                    SDL_QueryTexture(rTexture, NULL, NULL, &wi, &hi);
                    float w = wi, h = hi;

                    float sizeRatio = currentSprite->size / 100.0f;
                    w *= sizeRatio;
                    h *= sizeRatio;

                    int x = (int)round(currentSprite->x + ((float)ww/2.0f) - (w/2.0f));
                    int y = (int)round(-currentSprite->y + ((float)wh/2.0f) - (h/2.0f));

                    SDL_Rect dRect = { x, y, (int)round(w), (int)round(h) };
                    SDL_Point centerRot = { currentCostume->rotationCenterX * sizeRatio, currentCostume->rotationCenterY * sizeRatio };

                    float angle = (currentSprite->direction - 90.0f);

                    //printf("Rendering sprite %d at %f;%f\n", i, currentSprite->x, currentSprite->y);
                    //printf("Rendering Sprite %d at %d;%d, sized %f;%f, angled %f, centered at %d;%d\n", i, x, y, w, h, angle, centerRot.x, centerRot.y);

                    SDL_RendererFlip flip = SDL_FLIP_NONE;
                    if (strcmp(currentSprite->rotationStyle, "don't rotate") == 0) {
                        angle = 0.0f;
                    }
                    else if (strcmp(currentSprite->rotationStyle, "left-right") == 0) {
                        float angleMod = angle;
                        while (angleMod < 0.0f) angleMod += 360.0f;
                        while (angleMod > 360.0f) angleMod -= 360.0f;

                        if (angleMod > 90.0f) flip = SDL_FLIP_HORIZONTAL;
                    }

                    SDL_RenderCopyEx(Game::instance->renderer, rTexture, NULL, &dRect, angle, &centerRot, flip);
                }
            }
        }
    }

    if (!isRunning) {
        int ww, wh;
        SDL_GetWindowSize(Game::instance->window, &ww, &wh);

        SDL_Rect dRect = { 0, 0, ww, wh };
        SDL_RenderCopy(Game::instance->renderer, this->hangOn, NULL, &dRect);
    }
}