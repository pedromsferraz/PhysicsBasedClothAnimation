#include <glm/glm.hpp>

struct Particle {
    float mass;
    glm::vec3 previousPosition;
    glm::vec3 position;
    bool isFixed;

    Particle(float mass, glm::vec3 position, bool isFixed);
};
