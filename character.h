#pragma once

#include <SDL2/SDL.h>

#include "graphics.h"

struct Object {
    SDL_FRect rect;
    Graphics::Texture t;
    float hitbox_x, hitbox_y, hitbox_width, hitbox_height;

    Object() {}

    Object(const char *file, float scale, float spawn_x, float spawn_y,
           float hx = 0, float hy = 0, float hw = 0, float hh = 0)
        : hitbox_width(hw), hitbox_height(hh) {
        t = Graphics::loadTexture(file, scale);
        rect = {spawn_x, spawn_y - t.height, (float)t.width, (float)t.height};
        hitbox_x = hx;
        hitbox_y = hy;

        if (hw == 0) {
            hitbox_width = t.width;
        }
        if (hh == 0) {
            hitbox_height = t.height;
        }
    }

    void move(float dx, float dy) {
        rect.x -= dx;
        rect.y -= dy;
    }

    bool collided(const Object &c) {
        if ((rect.x + hitbox_x + hitbox_width < c.rect.x + c.hitbox_x) ||
            (c.rect.x + c.hitbox_x + c.hitbox_width < rect.x + hitbox_x)) {
            return false;
        }
        if ((rect.y + hitbox_y + hitbox_height < c.rect.y + c.hitbox_y) ||
            (c.rect.y + c.hitbox_y + c.hitbox_height < rect.y + hitbox_y)) {
            return false;
        }
        return true;
    }

    void draw(SDL_Renderer *renderer) {
        SDL_RenderCopyF(renderer, t.texture, NULL, &rect);
    }
};

struct Player {
    enum State { RUN_1 = 0, RUN_2 = 1, JUMP = 2, CROUTCH = 3 };

    static const Uint64 totalStateMs = 750;
    static const Uint64 runningStateMs = 125;

    Object playerTextures[4];

    Uint64 remainingStateMs, lastMs;

    State currentState;

    float positionY;

    Player(float x, float y) {
        playerTextures[RUN_1] =
            Object("sprites/behula/behula_1.png", 0.5, x, y);
        playerTextures[RUN_2] =
            Object("sprites/behula/behula_2.png", 0.5, x, y);
        playerTextures[JUMP] =
            Object("sprites/behula/behula_3.png", 0.5, x, y - 120);
        playerTextures[CROUTCH] =
            Object("sprites/behula/behula_4.png", 0.5, x, y);
        currentState = RUN_1;
        remainingStateMs = runningStateMs;
        lastMs = SDL_GetTicks64();
        positionY = y;
    }

    bool collided(const Object &c) {
        return playerTextures[currentState].collided(c);
    }

    void jump() {
        if (currentState == JUMP)
            remainingStateMs += totalStateMs;
        else {
            lastMs = SDL_GetTicks64();
            remainingStateMs = totalStateMs;
        }
        currentState = JUMP;
    }

    void croutch() {
        if (currentState == CROUTCH)
            remainingStateMs += totalStateMs;
        else {
            lastMs = SDL_GetTicks64();
            remainingStateMs = totalStateMs;
        }
        currentState = CROUTCH;
    }

    void draw(SDL_Renderer *renderer) {
        Uint64 currentMs = SDL_GetTicks64();
        Uint64 elapsed = currentMs - lastMs;
        if (remainingStateMs <= elapsed) {
            State nextState = RUN_1;
            switch (currentState) {
                case RUN_1:
                    nextState = RUN_2;
                    remainingStateMs = runningStateMs;
                    break;
                case RUN_2:
                    nextState = RUN_1;
                    remainingStateMs = runningStateMs;
                    break;
                case JUMP:
                case CROUTCH:
                    nextState = RUN_1;
                    remainingStateMs = runningStateMs;
                    break;
            }
            currentState = nextState;
        } else {
            remainingStateMs -= elapsed;
        }
        playerTextures[currentState].draw(renderer);
        lastMs = currentMs;
    }
};

struct CharacterAI {
    static void spawnObjects(float height);
    static void drawAll(SDL_Renderer *renderer, float dx);
};
