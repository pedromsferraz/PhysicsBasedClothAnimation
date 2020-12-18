#include <glm/glm.hpp>
#include <iostream>
#include <vector>
#include <set>
#include "mesh.h"

using edge = std::pair<std::pair<int, int>, std::pair<int, int> >;

class RectangularMesh : public Mesh {
    std::set<edge> edges;
    bool inBounds(int n, int m, int i, int j);
    void createBarIfNotExist(int n, int m, int i, int j, int k, int l);

public:
    int n, m;
    std::vector<std::vector<Particle> > particles;

    RectangularMesh(int n, int m,
                    float mass,
                    float barLength,
                    int n_relaxations,
                    float h,
                    float delta,
                    glm::vec3 force,
                    glm::vec3 initialVelocity);
    void oneStep();
    void oneStep(float h, float delta, glm::vec3 force);
};
