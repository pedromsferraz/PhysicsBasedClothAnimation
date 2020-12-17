#include "bar.h"
#include <glm/glm.hpp>

Bar::Bar(Particle &p1, Particle &p2, float length, float tolerance) 
    : p1(p1), p2(p2), length(length), tolerance(tolerance) { }

bool Bar::update() {
    glm::vec3 direction = p1.position - p2.position;
    float distance = glm::length(direction);
    float adjust = length - distance;

    if (fabs(adjust) < tolerance) {
        return true;
    }

    direction /= distance;
    if (!p1.isFixed && !p2.isFixed) {
        p1.position += (adjust / 2.0f) * direction;
        p2.position += (adjust / 2.0f) * (-direction);
    } else if (!p1.isFixed) {
        p1.position += adjust * direction;
    } else if (!p2.isFixed) {
        p2.position += adjust * (-direction);
    }
    return false;
}
