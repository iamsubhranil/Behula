#include "character.h"

#include <deque>

std::deque<Object> loadedObjects = std::deque<Object>();

struct Enemy {
    const char *filename;
    float scale;
    int max_staggered;
    float hx, hy, hw, hh;
};

Enemy enemies[] = {{"sprites/enemy/enemy_1.png", 0.5, 1, 0, 0, 0, 250},
                   {"sprites/enemy/enemy_2.png", 0.5, 2, 0, 0, 0, 0},
                   {"sprites/enemy/enemy_3.png", 0.5, 1, 0, 0, 0, 250},
                   {"sprites/enemy/enemy_4.png", 0.5, 1, 0, 0, 0, 250}};

void CharacterAI::spawnObjects(float height) {
    bool shouldSpawn = (rand() % 119) == 57;

    if (!shouldSpawn) return;

    if (loadedObjects.size() > 0) {
        const Object &lastObj = loadedObjects.back();
        if (lastObj.rect.x + lastObj.rect.w > Graphics::WIDTH - 400) {
            return;
        }
    }

    int num_enemies = sizeof(enemies) / sizeof(Enemy);

    Enemy e = enemies[rand() % num_enemies];

    float startx = Graphics::WIDTH;

    loadedObjects.push_back(
        Object(e.filename, e.scale, startx, height, e.hx, e.hy, e.hw, e.hh));

    if (e.max_staggered > 1) {
        shouldSpawn = (rand() % 119) == 69;
        if (!shouldSpawn) return;

        loadedObjects.push_back(Object(e.filename, e.scale, startx + e.hw,
                                       height, e.hx, e.hy, e.hw, e.hh));
    }
}

void CharacterAI::drawAll(SDL_Renderer *renderer, float dx) {
    if (loadedObjects.size() > 0) {
        for (auto &a : loadedObjects) {
            a.move(dx, 0);
        }

        while (loadedObjects.front().rect.x + loadedObjects.front().rect.w <
               0) {
            loadedObjects.pop_front();
        }

        if (loadedObjects.size() == 0) return;

        for (auto &a : loadedObjects) {
            a.draw(renderer);
        }
    }
}

bool CharacterAI::checkCollision(Player &p) {
    for (const auto &o : loadedObjects) {
        if (p.collided(o)) return true;
    }
    return false;
}
