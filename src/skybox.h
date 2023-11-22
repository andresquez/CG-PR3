#include <string>
#include <glm/glm.hpp>
#include "color.h"
#include <SDL_image.h> // Include this for SDL_Surface

class Skybox {
public:
    Skybox(const std::string& textureFile);
    ~Skybox();

    Color getColor(const glm::vec3& direction) const;
    bool rayIntersect(const glm::vec3& origin, const glm::vec3& direction) const; 

private:
    SDL_Surface* texture;
    void loadTexture(const std::string& textureFile);
};
