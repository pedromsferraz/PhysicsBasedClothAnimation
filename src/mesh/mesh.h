#include <glm/glm.hpp>
#include <iostream>
#include <vector>
#include <set>
#include "bar.h"

// Class that represents the mesh. Contains a vector with it's bars,
// a set that registers the already existing bars (edge), the force that
// acts on the mesh, number of relaxations for each bar in one step,
// the size of the step and the damping coefficient.
class Mesh {
public:
    std::vector<Bar> bars;
    glm::vec3 force;
    int n_relaxations;
    float h;
    float delta;

    // Sets the force that acts on the mesh.
    void setForce(glm::vec3 force);

    // Implementation of oneStep without receiving paramenters.
    void oneStep();

    // Receives the step, the damping coefficient and the force that acts on the mesh
    // and calculates the next position of each particle.
    void oneStep(float h, float delta, glm::vec3 force);
};
