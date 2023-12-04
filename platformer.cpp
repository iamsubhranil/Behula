#include "platformer.h"

#include <SDL2/SDL.h>

#include <cstdio>
#include <cstdlib>
#include <deque>
#include <tuple>
#include <unordered_map>
#include <vector>

#include "graphics.h"

auto layerTextures =
    std::unordered_map<const char *,
                       std::tuple<std::vector<Graphics::Texture>, int>>();

auto texturesPresent =
    std::unordered_map<const char *,
                       std::deque<std::tuple<Graphics::Texture, SDL_FRect>>>();

void Platformer::addLayer(const char *layer, const char *prefix, int count,
                          int height, double scale) {
    auto allTextures = std::vector<Graphics::Texture>();
    for (int i = 0; i < count; i++) {
        char file[200];
        sprintf(file, "%s%d.png", prefix, i + 1);
        allTextures.push_back(Graphics::loadTexture(file, scale));
    }
    std::get<0>(layerTextures[layer]) = allTextures;
    std::get<1>(layerTextures[layer]) = height;

    texturesPresent[layer] =
        std::deque<std::tuple<Graphics::Texture, SDL_FRect>>();
    auto &onScreenTextures = texturesPresent[layer];
    Graphics::Texture t = allTextures[rand() % count];
    onScreenTextures.push_back(
        {t,
         {0.0, (float)(height - t.height), (float)t.width, (float)t.height}});
}

void Platformer::drawLayer(SDL_Renderer *renderer, const char *layer,
                           float dx) {
    auto &onScreenTextures = texturesPresent[layer];

    for (auto &t : onScreenTextures) {
        std::get<1>(t).x -= dx;
    }

    while (std::get<1>(onScreenTextures.front()).x +
               std::get<1>(onScreenTextures.front()).w <
           0) {
        onScreenTextures.pop_front();
    }

    const auto &allTextures = std::get<0>(layerTextures[layer]);
    const int layerHeight = std::get<1>(layerTextures[layer]);

    while (std::get<1>(onScreenTextures.back()).x +
               std::get<1>(onScreenTextures.back()).w <
           Graphics::WIDTH + 50) {  // + 50 will preload textures
        float x = std::get<1>(onScreenTextures.back()).x +
                  std::get<1>(onScreenTextures.back()).w -
                  20;  // - 20 will make subsequent textures overlap

        Graphics::Texture t = allTextures[rand() % allTextures.size()];
        onScreenTextures.push_back({t,
                                    {(float)x, (float)(layerHeight - t.height),
                                     (float)t.width, (float)t.height}});
    }

    for (const auto &t : onScreenTextures) {
        Graphics::Texture g = std::get<0>(t);
        SDL_FRect rect = std::get<1>(t);
        SDL_RenderCopyF(renderer, g.texture, NULL, &rect);
    }
}
