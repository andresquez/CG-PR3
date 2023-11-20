#include "cube.h"

Cube::Cube(const glm::vec3& pos, float s, const Material& mat)
    : position(pos), size(s), Object(mat) {}

Intersect Cube::rayIntersect(const glm::vec3& rayOrigin, const glm::vec3& rayDirection) const {
    // Dimensiones del cubo
    float minX = position.x - size / 2.0f;
    float maxX = position.x + size / 2.0f;
    float minY = position.y - size / 2.0f;
    float maxY = position.y + size / 2.0f;
    float minZ = position.z - size / 2.0f;
    float maxZ = position.z + size / 2.0f;

    // Algoritmo de intersección de rayos con un cubo (Moller–Trumbore)
    glm::vec3 invDir = 1.0f / rayDirection;
    glm::vec3 tMin = (glm::vec3(minX, minY, minZ) - rayOrigin) * invDir;
    glm::vec3 tMax = (glm::vec3(maxX, maxY, maxZ) - rayOrigin) * invDir;

    glm::vec3 t1 = glm::min(tMin, tMax);
    glm::vec3 t2 = glm::max(tMin, tMax);

    float tNear = glm::max(glm::max(t1.x, t1.y), t1.z);
    float tFar = glm::min(glm::min(t2.x, t2.y), t2.z);

    if (tNear > tFar || tFar < 0.0f) {
        return Intersect{false};
    }

    glm::vec3 point = rayOrigin + tNear * rayDirection;
    glm::vec3 normal;

    if (t1.x > t1.y && t1.x > t1.z) {
        normal = (rayDirection.x < 0) ? glm::vec3(1, 0, 0) : glm::vec3(-1, 0, 0);
    } else if (t1.y > t1.z) {
        normal = (rayDirection.y < 0) ? glm::vec3(0, 1, 0) : glm::vec3(0, -1, 0);
    } else {
        normal = (rayDirection.z < 0) ? glm::vec3(0, 0, 1) : glm::vec3(0, 0, -1);
    }

    return Intersect{true, tNear, point, normal};
}
