#include <glm/glm.hpp>
#include <iostream>
#include <vector>
#include <set>
#include "bar.h"

using edge = std::pair<std::pair<int, int>, std::pair<int, int> >;

class Mesh {
    std::vector<std::vector<Particle> > previousParticles;
    std::vector<Bar> bars;
    std::set<edge> edges;
    glm::vec3 force;
    float tolerance = 2e-1;

    bool inBounds(int n, int m, int i, int j);
    void createBarIfNotExist(int n, int m, int i, int j, int k, int l);

public:
    int n, m;
    std::vector<std::vector<Particle> > particles;

    Mesh(int n, int m, float mass, float barLength);
    void setForce(glm::vec3 force);
    void oneStep(float h, float delta, glm::vec3 force);
    void oneStep(float h, float delta);
    void simulate();
};
