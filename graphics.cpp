// for initializing and shutdown functions
#include <SDL2/SDL.h>
// for rendering images and graphics on screen
#include <SDL2/SDL_image.h>
// for using SDL_Delay() functions
#include <SDL2/SDL_timer.h>
// for font functions
#include <SDL2/SDL_ttf.h>

#include <vector>

#include "character.h"
#include "graphics.h"
#include "platformer.h"

SDL_Window* Graphics::window = NULL;
SDL_Renderer* Graphics::renderer = NULL;

void Graphics::init() {
    // returns zero on success else non-zero
    if (SDL_Init(SDL_INIT_VIDEO) != 0) {
        printf("[Error] Unable to init SDL: %s\n", SDL_GetError());
        abort();
    }

    const Uint32 RENDER_FLAGS = SDL_RENDERER_ACCELERATED;

    window = SDL_CreateWindow("Behula", SDL_WINDOWPOS_CENTERED,
                              SDL_WINDOWPOS_CENTERED, WIDTH, HEIGHT, 0);
    renderer = SDL_CreateRenderer(window, -1, RENDER_FLAGS);

    TTF_Init();

    Platformer::addLayer("bush", "sprites/bush/bush_shadow_", 10, 700, 0.4);
    Platformer::addLayer("sky", "sprites/sky/sky_", 1, 680, 1.5);
    Platformer::addLayer("ground", "sprites/ground/ground_", 1, HEIGHT + 630,
                         1);
    /*
    if (IMG_Init(IMG_INIT_PNG) != 0) {
        printf("error initializing IMG: %s\n", SDL_GetError());
        abort();
    }*/
}

struct TextInfo {
    SDL_Texture* textTexture;
    SDL_Rect destRect;
};

TextInfo drawText(SDL_Renderer* renderer, TTF_Font* FONT, const char* str,
                  TextInfo& lastInfo) {
    if (lastInfo.textTexture) {
        SDL_DestroyTexture(lastInfo.textTexture);
    }

    SDL_Color foregroundColor = {255, 255, 255, 255};

    SDL_Surface* textSurface = TTF_RenderText_Solid(FONT, str, foregroundColor);

    SDL_Texture* textTexture =
        SDL_CreateTextureFromSurface(renderer, textSurface);
    SDL_Rect destRect = {lastInfo.destRect.x, lastInfo.destRect.y,
                         textSurface->w, textSurface->h};

    SDL_FreeSurface(textSurface);
    return {textTexture, destRect};
}

void Graphics::run() {
    bool keys[322] = {false};
    bool close = false;
    // measure fps
    char fpsStr[50] = {'.', '.', '.'};

    TTF_Font* fpsFont = TTF_OpenFont("font.ttf", 20);
    TextInfo fpsInfo = {NULL, {0, 0, 0, 0}};
    Uint64 lastText = 0;

    TTF_Font* scoreFont = TTF_OpenFont("font.ttf", 30);
    TextInfo scoreInfo = {NULL, {200, 0, 0, 0}};
    long lastScore = -1;

    Player player(30, 710);

    while (!close) {
        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            switch (event.type) {
                case SDL_QUIT:
                    close = 1;
                    break;
                case SDL_KEYDOWN: {
                    if (event.key.keysym.sym < 322)
                        keys[event.key.keysym.sym] = true;
                    break;
                };
                case SDL_KEYUP: {
                    if (event.key.keysym.sym < 322)
                        keys[event.key.keysym.sym] = false;
                    break;
                };
            }
        }
        SDL_RenderClear(renderer);
        // measure fps
        Uint64 lastTick = SDL_GetTicks64();
        const float movement_speed = 2;
        Platformer::drawLayer(renderer, "sky", movement_speed);
        Platformer::drawLayer(renderer, "ground", movement_speed);
        Platformer::drawLayer(renderer, "bush", movement_speed);
        if (keys[SDLK_w]) {
            player.jump();
            keys[SDLK_w] = false;
        } else if (keys[SDLK_s]) {
            player.croutch();
            keys[SDLK_s] = false;
        }
        CharacterAI::spawnObjects(700);
        CharacterAI::drawAll(renderer, movement_speed);
        player.draw(renderer);
        Uint64 currentTick = SDL_GetTicks64();
        // draw fps
        if (currentTick - lastText > 500) {
            Uint64 ms = currentTick - lastTick;
            if (ms > 0)
                sprintf(fpsStr, "%4lums %5lufps", ms, 1000 / ms);
            else
                strcpy(fpsStr, "0ms");
            lastText = currentTick;
            fpsInfo = drawText(renderer, fpsFont, fpsStr, fpsInfo);
        }
        if (player.getScore() != lastScore) {
            lastScore = player.getScore();
            sprintf(fpsStr, "Score: %lu", lastScore);
            scoreInfo = drawText(renderer, scoreFont, fpsStr, scoreInfo);
        }
        SDL_RenderCopy(renderer, fpsInfo.textTexture, NULL, &fpsInfo.destRect);
        SDL_RenderCopy(renderer, scoreInfo.textTexture, NULL,
                       &scoreInfo.destRect);
        SDL_RenderPresent(renderer);
        if (CharacterAI::checkCollision(player)) {
            close = true;
            printf("[GameOver] Collision!\n");
        }
    }
}

Graphics::Texture Graphics::loadTexture(const char* file, double scale) {
    Texture t;

    t.texture = IMG_LoadTexture(renderer, file);
    if (t.texture == NULL) {
        printf("[Error] Failed to load texture '%s'!\n", file);
        abort();
    }
    SDL_QueryTexture(t.texture, NULL, NULL, &t.width, &t.height);

    t.width = (int)((double)t.width * scale);
    t.height = (int)((double)t.height * scale);

    return t;
}

void Graphics::destroy() {
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
}
