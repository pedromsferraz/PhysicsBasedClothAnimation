#include <glm/glm.hpp>
#include <iostream>
#include <vector>
#include <set>
#include "bar.h"

class Mesh {
public:
    std::vector<Bar> bars;
    glm::vec3 force;
    int n_relaxations;
    float h;
    float delta;

    void setForce(glm::vec3 force);
    void oneStep();
    void oneStep(float h, float delta, glm::vec3 force);
};
