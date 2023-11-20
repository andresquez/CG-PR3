#include <SDL2/SDL.h>
#include <SDL_events.h>
#include <SDL_render.h>
#include <cstdlib>
#include <glm/ext/quaternion_geometric.hpp>
#include <glm/geometric.hpp>
#include <string>
#include <glm/glm.hpp>
#include <vector>
#include "print.h"
#include "cube.h"  
#include "color.h"
#include "intersect.h"
#include "object.h"
#include "sphere.h"
#include "light.h"
#include "camera.h"
#include "SOIL.h"

const int SCREEN_WIDTH = 800;
const int SCREEN_HEIGHT = 600;
const float ASPECT_RATIO = static_cast<float>(SCREEN_WIDTH) / static_cast<float>(SCREEN_HEIGHT);
const int MAX_RECURSION = 3;
const float BIAS = 0.0001f;

SDL_Renderer* renderer;
std::vector<Object*> objects;
Light light(glm::vec3(-1.0, 0, 10), 1.5f, Color(255, 255, 255));
Camera camera(glm::vec3(0.0, 0.0, 5.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f), 10.0f);


void point(glm::vec2 position, Color color) {
    SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a);
    SDL_RenderDrawPoint(renderer, position.x, position.y);
}

float castShadow(const glm::vec3& shadowOrigin, const glm::vec3& lightDir, Object* hitObject) {
    for (auto& obj : objects) {
        if (obj != hitObject) {
            Intersect shadowIntersect = obj->rayIntersect(shadowOrigin, lightDir);
            if (shadowIntersect.isIntersecting && shadowIntersect.dist > 0) {
                float shadowRatio = shadowIntersect.dist / glm::length(light.position - shadowOrigin);
                shadowRatio = glm::min(1.0f, shadowRatio);
                return 1.0f - shadowRatio;
            }
        }
    }
    return 1.0f;
}

Color castRay(const glm::vec3& rayOrigin, const glm::vec3& rayDirection, const short recursion = 0) {
    float zBuffer = 99999;
    Object* hitObject = nullptr;
    Intersect intersect;

    for (const auto& object : objects) {
        Intersect i = object->rayIntersect(rayOrigin, rayDirection);
        if (i.isIntersecting && i.dist < zBuffer) {
            zBuffer = i.dist;
            hitObject = object;
            intersect = i;
        }
    }

    if (!intersect.isIntersecting || recursion == MAX_RECURSION) {
        return Color(173, 216, 230);
    }


    glm::vec3 lightDir = glm::normalize(light.position - intersect.point);
    glm::vec3 viewDir = glm::normalize(rayOrigin - intersect.point);
    glm::vec3 reflectDir = glm::reflect(-lightDir, intersect.normal); 

    float shadowIntensity = castShadow(intersect.point, lightDir, hitObject);

    float diffuseLightIntensity = std::max(0.0f, glm::dot(intersect.normal, lightDir));
    float specReflection = glm::dot(viewDir, reflectDir);
    
    Material mat = hitObject->material;

    float specLightIntensity = std::pow(std::max(0.0f, glm::dot(viewDir, reflectDir)), mat.specularCoefficient);


    Color reflectedColor(0.0f, 0.0f, 0.0f);
    if (mat.reflectivity > 0) {
        glm::vec3 origin = intersect.point + intersect.normal * BIAS;
        reflectedColor = castRay(origin, reflectDir, recursion + 1); 
    }

    Color refractedColor(0.0f, 0.0f, 0.0f);
    if (mat.transparency > 0) {
        glm::vec3 origin = intersect.point - intersect.normal * BIAS;
        glm::vec3 refractDir = glm::refract(rayDirection, intersect.normal, mat.refractionIndex);
        refractedColor = castRay(origin, refractDir, recursion + 1); 
    }



    Color diffuseLight = mat.diffuse * light.intensity * diffuseLightIntensity * mat.albedo * shadowIntensity;
    Color specularLight = light.color * light.intensity * specLightIntensity * mat.specularAlbedo * shadowIntensity;
    Color color = (diffuseLight + specularLight) * (1.0f - mat.reflectivity - mat.transparency) + reflectedColor * mat.reflectivity + refractedColor * mat.transparency;
    return color;
} 


void createMinecraftScene() {
    // Agrega el suelo
    Material dirt = {
        Color(139, 69, 19),   // Diffuse color for dirt
        0.8f,                  // Diffuse coefficient
        0.2f,                  // Specular coefficient
        10.0f,                 // Specular exponent
        0.0f,                  // Reflectivity (not reflective)
        0.0f                   // Transparency (not transparent)
    };
    objects.push_back(new Cube(glm::vec3(0.0f, -2.0f, 0.0f), 1.0f, dirt));

    // Agrega bloques de agua, grama, roca, nieve y lava
    Material water = { Color(70, 130, 180), 0.8f, 0.2f, 50.0f, 0.1f, 0.9f };
    objects.push_back(new Cube(glm::vec3(-2.0f, 0.0f, -2.0f), 1.0f, water));

    Material grass = { Color(50, 205, 50), 0.8f, 0.2f, 30.0f, 0.0f, 0.0f };
    objects.push_back(new Cube(glm::vec3(-1.0f, 0.0f, -2.0f), 1.0f, grass));

    Material rock = { Color(169, 169, 169), 0.7f, 0.3f, 20.0f, 0.2f, 0.0f };
    objects.push_back(new Cube(glm::vec3(0.0f, 0.0f, -2.0f), 1.0f, rock));

    Material snow = { Color(255, 250, 250), 0.9f, 0.1f, 50.0f, 0.8f, 0.0f };
    objects.push_back(new Cube(glm::vec3(1.0f, 0.0f, -2.0f), 1.0f, snow));

    Material lava = { Color(255, 69, 0), 0.9f, 0.1f, 30.0f, 0.7f, 0.0f };
    objects.push_back(new Cube(glm::vec3(2.0f, 0.0f, -2.0f), 1.0f, lava));

    // Agrega algunos elementos más
    Material diamond = { Color(0, 255, 0), 0.0f, 10.0f, 1425.0f, 0.0f, 0.0f };
    objects.push_back(new Cube(glm::vec3(0.0f, 1.0f, 0.0f), 1.0f, diamond));

    Material rubber = { Color(80, 0, 0), 0.9, 0.1, 10.0f, 0.0f, 0.0f };
    objects.push_back(new Cube(glm::vec3(-1.0f, 1.0f, 0.0f), 1.0f, rubber));

    Material ivory = { Color(100, 100, 80), 0.5, 0.5, 50.0f, 0.4f, 0.0f };
    objects.push_back(new Cube(glm::vec3(1.0f, 1.0f, 0.0f), 1.0f, ivory));

    Material mirror = { Color(255, 255, 255), 0.0f, 10.0f, 1425.0f, 0.9f, 0.0f };
    objects.push_back(new Cube(glm::vec3(-2.0f, 1.0f, 0.0f), 1.0f, mirror));

    Material glass = { Color(255, 255, 255), 0.0f, 10.0f, 1425.0f, 0.2f, 1.0f };
    objects.push_back(new Cube(glm::vec3(2.0f, 1.0f, 0.0f), 1.0f, glass));
}


void setUp() {

    // light.position = glm::vec3(0.0f, 0.0f, 5.0f);
    light.intensity = 2.0f;
    light.color = Color(255, 255, 255);


    Material rubber = {
        Color(80, 0, 0),   // diffuse color means the color of the object under white light, in this case it is red but it will look like orange
        0.9,
        0.1,
        10.0f,
        0.0f,
        0.0f
    };

    Material ivory = {
        Color(100, 100, 80),
        0.5,
        0.5,
        50.0f,
        0.4f,
        0.0f
    };

    Material mirror = {
        Color(255, 255, 255),
        0.0f,
        10.0f,
        1425.0f,
        0.9f,
        0.0f
    };

    Material glass = {
        Color(255, 255, 255),
        0.0f,
        10.0f,
        1425.0f,
        0.2f,
        1.0f,
    };

    // Agrega un cubo a la escena
    Material dirt = {
        Color(139, 69, 19),   // Diffuse color
        0.8f,                  // Diffuse coefficient
        0.2f,                  // Specular coefficient
        10.0f,                // Specular exponent
        0.0f,                 // Reflectivity (no refleja)
        0.0f                  // Transparency (no transparente)
    };

    // Diamond material
    Material diamond = {
        Color(255, 0, 0),
        0.0f,
        10.0f,
        1425.0f,
        0.0f,
        0.0f
    };

    Material water = {
        Color(30, 10, 180),   // Diffuse color for water
        1.0f,                  // Diffuse coefficient
        1.0f,                  // Specular coefficient
        50.0f,                 // Specular exponent, specular is the light that reflects off the surface, the higher the value the more it reflects, meaning that its less vis
        0.3f,                  // Reflectivity (somewhat reflective)
        0.3f                   // Transparency (mostly transparent)
    };

    Material grass = {
        Color(50, 205, 50),    // Diffuse color for grass
        0.8f,                  // Diffuse coefficient
        0.2f,                  // Specular coefficient
        30.0f,                 // Specular exponent
        0.0f,                  // Reflectivity (not reflective)
        0.0f                   // Transparency (not transparent)
    };

    Material rock = {
        Color(169, 169, 169),  // Diffuse color for rock
        0.7f,                  // Diffuse coefficient
        0.3f,                  // Specular coefficient
        20.0f,                 // Specular exponent
        0.2f,                  // Reflectivity (somewhat reflective)
        0.0f                   // Transparency (not transparent)
    };

    Material snow = {
        Color(255, 250, 250),  // Diffuse color for snow
        0.9f,                  // Diffuse coefficient
        0.1f,                  // Specular coefficient
        50.0f,                 // Specular exponent
        0.8f,                  // Reflectivity (highly reflective)
        0.0f                   // Transparency (not transparent)
    };

    Material lava = {
        Color(255, 69, 0),     // Diffuse color for lava
        0.9f,                  // Diffuse coefficient
        0.1f,                  // Specular coefficient
        30.0f,                 // Specular exponent
        0.7f,                  // Reflectivity (reflective)
        0.0f                   // Transparency (not transparent)
    };

    Material wood = {
        Color(139, 69, 19),   // Diffuse color for wood
        0.8f,                  // Diffuse coefficient
        0.2f,                  // Specular coefficient
        10.0f,                 // Specular exponent
        0.0f,                  // Reflectivity (no refleja)
        0.0f                   // Transparency (no transparente)
    };


        // Create the ground
    for (int i = -3; i <= 3; ++i) {
        for (int j = -3; j <= 3; ++j) {
            objects.push_back(new Cube(glm::vec3(i, -2.0f, j), 1.0f, dirt));
        }
    }


    // Create a pond
    for (int i = -2; i <= 2; ++i) {
        for (int j = -2; j <= 2; ++j) {
            objects.push_back(new Cube(glm::vec3(i, -1.5f, j), 1.0f, water));
        }
    }

    // add grass around the pond 
    for (int i = -3; i <= 3; ++i) {
        for (int j = -3; j <= 3; ++j) {
            objects.push_back(new Cube(glm::vec3(i, -1.5f, j), 1.0f, grass));
        }
    }

}

void render() {
    float fov = 3.1415/3;
    for (int y = 0; y < SCREEN_HEIGHT; y++) {
        for (int x = 0; x < SCREEN_WIDTH; x++) {
            /*
            float random_value = static_cast<float>(std::rand())/static_cast<float>(RAND_MAX);
            if (random_value < 0.0) {
                continue;
            }
            */


            float screenX = (2.0f * (x + 0.5f)) / SCREEN_WIDTH - 1.0f;
            float screenY = -(2.0f * (y + 0.5f)) / SCREEN_HEIGHT + 1.0f;
            screenX *= ASPECT_RATIO;
            screenX *= tan(fov/2.0f);
            screenY *= tan(fov/2.0f);


            glm::vec3 cameraDir = glm::normalize(camera.target - camera.position);

            glm::vec3 cameraX = glm::normalize(glm::cross(cameraDir, camera.up));
            glm::vec3 cameraY = glm::normalize(glm::cross(cameraX, cameraDir));
            glm::vec3 rayDirection = glm::normalize(
                cameraDir + cameraX * screenX + cameraY * screenY
            );
           
            Color pixelColor = castRay(camera.position, rayDirection);
            /* Color pixelColor = castRay(glm::vec3(0,0,20), glm::normalize(glm::vec3(screenX, screenY, -1.0f))); */

            point(glm::vec2(x, y), pixelColor);
        }
    }
}

int main(int argc, char* argv[]) {
    // Initialize SDL
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        SDL_Log("Unable to initialize SDL: %s", SDL_GetError());
        return 1;
    }

    // Create a window
    SDL_Window* window = SDL_CreateWindow("Hello World - FPS: 0", 
                                          SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 
                                          SCREEN_WIDTH, SCREEN_HEIGHT, 
                                          SDL_WINDOW_SHOWN);

    if (!window) {
        SDL_Log("Unable to create window: %s", SDL_GetError());
        SDL_Quit();
        return 1;
    }

    // Create a renderer
    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

    if (!renderer) {
        SDL_Log("Unable to create renderer: %s", SDL_GetError());
        SDL_DestroyWindow(window);
        SDL_Quit();
        return 1;
    }

    bool running = true;
    SDL_Event event;

    int frameCount = 0;
    Uint32 startTime = SDL_GetTicks();
    Uint32 currentTime = startTime;
    
    setUp();
    // createMinecraftScene();

    while (running) {
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                running = false;
            }

            if (event.type == SDL_KEYDOWN) {
                switch(event.key.keysym.sym) {
                    case SDLK_UP:
                        camera.move(-1.0f);
                        break;
                    case SDLK_DOWN:
                        camera.move(1.0f);
                        break;
                    case SDLK_LEFT:
                        print("left");
                        camera.rotate(-1.0f, 0.0f);
                        break;
                    case SDLK_RIGHT:
                        print("right");
                        camera.rotate(1.0f, 0.0f);
                        break;
                    case SDLK_w:
                        camera.position.y += 0.5f;  // Ajusta el valor según sea necesario
                        break;
                    case SDLK_s:
                        camera.position.y -= 0.5f;  // Ajusta el valor según sea necesario
                        break;
                }
            }

        }

        light.position = camera.position;

        // Clear the screen
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderClear(renderer);

        render();

        // Present the renderer
        SDL_RenderPresent(renderer);

        frameCount++;

        // Calculate and display FPS
        if (SDL_GetTicks() - currentTime >= 1000) {
            currentTime = SDL_GetTicks();
            std::string title = "Hello World - FPS: " + std::to_string(frameCount);
            SDL_SetWindowTitle(window, title.c_str());
            frameCount = 0;
        }
    }

    // Cleanup
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}
