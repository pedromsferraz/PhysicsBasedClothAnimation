#include <glm/glm.hpp>

// Struct respsonsible for representing the particle. Contains
// it's mass, a vector with the previous position of the particle
// a vector with it's current position and a boolean that indicates
// wheter it is fixed or not.
struct Particle {
    float mass;
    glm::vec3 previousPosition;
    glm::vec3 position;
    bool isFixed;

    Particle(float mass, glm::vec3 position, bool isFixed);
};
