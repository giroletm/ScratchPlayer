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
}

Executor::~Executor() {
    delete targets;
    delete zfile;

    instance = 0;
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
                    int y = (int)round(currentSprite->y + ((float)wh/2.0f) - (h/2.0f));

                    SDL_Rect dRect = { x, y, (int)round(w), (int)round(h) };
                    SDL_Point centerRot = { currentCostume->rotationCenterX, currentCostume->rotationCenterY };

                    float angle = (currentSprite->direction - 90.0f);

                    //printf("Rendering Sprite %d at %d;%d, sized %f;%f, angled %f, centered at %d;%d\n", i, x, y, w, h, angle, centerRot.x, centerRot.y);

                    SDL_RenderCopyEx(Game::instance->renderer, rTexture, NULL, &dRect, angle, &centerRot, SDL_FLIP_NONE);
                }
            }
        }
    }
}