struct Light {
    glm::vec3 position;
    float intensity;
    Color color;

    // Constructor personalizado
    Light(const glm::vec3& pos, float inten, const Color& col)
        : position(pos), intensity(inten), color(col) {}
};
