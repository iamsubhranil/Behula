#pragma once

struct SDL_Renderer;

struct Platformer {
    static void addLayer(const char* name, const char* asset_prefix, int count,
                         int height, double scale = 1.0);

    static void drawLayer(SDL_Renderer* renderer, const char* layer, float dx);
};
