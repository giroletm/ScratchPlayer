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

    int spriteCount = targets->data.size();
    for (int spriteID = 0; spriteID < spriteCount; spriteID++) {
        Sprite* currentSprite = targets->sprites[spriteID];

        int blockSetCount = currentSprite->blockSets.size();
        for (int bSID = 0; bSID < blockSetCount; bSID++) {
            BlockSet* currentBS = currentSprite->blockSets[bSID];

            currentBS->execute(currentSprite);
        }
    }
}

void Executor::render() {
    int spriteCount = targets->data.size();
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
            if (currentSprite->layerOrder == layer) {
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

                    w *= currentSprite->size / 100.0f;
                    h *= currentSprite->size / 100.0f;

                    int x = (int)round(currentSprite->x + ((float)ww/2.0f) - (w/2.0f));
                    int y = (int)round(-currentSprite->y + ((float)wh/2.0f) - (h/2.0f));

                    SDL_Rect dRect = { x, y, (int)round(w), (int)round(h) };
                    SDL_Point centerRot = { currentCostume->rotationCenterX, currentCostume->rotationCenterY };

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