#include "skybox.h"
#include <cmath>
#include <stdexcept>
#include <algorithm>

Skybox::Skybox(const std::string& textureFile) {
    loadTexture(textureFile);
}

Skybox::~Skybox() {
    SDL_FreeSurface(texture);
}

void Skybox::loadTexture(const std::string& textureFile) {
    SDL_Surface* rawTexture = IMG_Load(textureFile.c_str());
    if (!rawTexture) {
        throw std::runtime_error("Failed to load skybox texture: " + std::string(IMG_GetError()));
    }
    texture = SDL_ConvertSurfaceFormat(rawTexture, SDL_PIXELFORMAT_RGB24, 0);
    if (!texture) {
        SDL_FreeSurface(rawTexture);
        throw std::runtime_error("Failed to convert skybox texture to RGB: " + std::string(SDL_GetError()));
    }
    SDL_FreeSurface(rawTexture);
}

Color Skybox::getColor(const glm::vec3& direction) const {
    float phi = atan2(direction.z, direction.x);
    float theta = acos(direction.y);

    float u = 0.5f + phi / (2 * M_PI);
    float v = theta / M_PI;

    int x = static_cast<int>(u * texture->w) % texture->w; 
    int y = static_cast<int>(v * texture->h) % texture->h;

    x = std::max(0, std::min(texture->w - 1, x));
    y = std::max(0, std::min(texture->h - 1, y));

    Uint8 r, g, b;
    Uint8* pixel = &((Uint8*)texture->pixels)[3 * (y * texture->w + x)];
    r = pixel[0];
    g = pixel[1];
    b = pixel[2];

    return Color(r, g, b);
}

bool Skybox::rayIntersect(const glm::vec3& origin, const glm::vec3& direction) const {
    // The size of the skybox
    float boxSize = 100.0f;

    // Calculate the half-size of the skybox
    float halfSize = boxSize * 0.5f;

    // Calculate the intersection distances to each face of the cube
    float tNear = (-halfSize - origin.x) / direction.x;
    float tFar = (halfSize - origin.x) / direction.x;
    float t1 = (-halfSize - origin.y) / direction.y;
    float t2 = (halfSize - origin.y) / direction.y;

    // Update the intersection distances based on y-axis intersections
    tNear = std::max(tNear, t1);
    tFar = std::min(tFar, t2);

    // Check if the ray misses the cube
    if (tNear > tFar || tFar < 0.0f) {
        return false;
    }

    // Update the intersection distances based on z-axis intersections
    float t3 = (-halfSize - origin.z) / direction.z;
    float t4 = (halfSize - origin.z) / direction.z;

    tNear = std::max(tNear, t3);
    tFar = std::min(tFar, t4);

    // Check if the ray misses the cube
    if (tNear > tFar || tFar < 0.0f) {
        return false;
    }

    // The ray intersects with the cube
    return true;
}

