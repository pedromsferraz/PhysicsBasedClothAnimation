#include <glm/glm.hpp>
#include "particle.h"

// Constructor responsible for creating the particles in each bar's end.
// Contains each particles mass, a 3 coordinate vector that contains
// its position and a boolean that indicates whether that particle is
// fixed or not.
Particle::Particle(float mass, glm::vec3 position, bool isFixed) 
    : mass(mass), position(position), isFixed(isFixed) { }
