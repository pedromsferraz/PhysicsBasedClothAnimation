#include "particle.h"
#include <glm/glm.hpp>

class Bar {
    Particle &p1;
    Particle &p2;
    float length;

public:
    Bar(Particle &p1, Particle &p2, float length);
    void update();
};
