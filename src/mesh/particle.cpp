#include <glm/glm.hpp>
#include "particle.h"

Particle::Particle(float mass, glm::vec3 position, bool isFixed) 
    : mass(mass), position(position), isFixed(isFixed) { }
