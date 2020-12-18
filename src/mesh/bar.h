#include "particle.h"
#include <glm/glm.hpp>

// Class that represents the bar. Constains two particles and the
// length of the bar (which is the distance between the particles).
// Each particle is in one end of the bar.
class Bar {
    Particle &p1;
    Particle &p2;
    float length;

public:

    // Constructor responsible for creating the bar.
    Bar(Particle &p1, Particle &p2, float length);

    // Method responsible for bar relaxation.
    void update();
};
