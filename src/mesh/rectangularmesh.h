#include <glm/glm.hpp>
#include <iostream>
#include <vector>
#include <set>
#include "mesh.h"

using edge = std::pair<std::pair<int, int>, std::pair<int, int> >;

// Class that represents a rectangular mesh. Contains a set of edges
// which serve to avoid creating duplicate bars.
class RectangularMesh : public Mesh {
    std::set<edge> edges;

    // Checks whether given indexes are in expected proportions.
    bool inBounds(int n, int m, int i, int j);

    // Receives two pairs of coordinates ( (n, m) and (i, j) ) and checks
    // whether an edge with those already exists.
    // Edges are used to avoid creating two identical bars.
    void createBarIfNotExist(int n, int m, int i, int j, int k, int l);

public:
    int n, m;
    std::vector<std::vector<Particle> > particles;

    // Rectangular mesh constructor. Creates a nxm mesh, receiving the mass to create the particles, the number
    // of relaxations that each bar does per step, the step size, the damping coefficient,
    // the force that acts on the mesh and the initial velocity of all non fix mesh particles.
    RectangularMesh(int n, int m,
                    float mass,
                    float barLength,
                    int n_relaxations,
                    float h,
                    float delta,
                    glm::vec3 force,
                    glm::vec3 initialVelocity);

    // Implementation of oneStep without receiving paramenters.s
    void oneStep();

    // Receives the step, the damping coefficient and the force that acts on the mesh
    // and calculates the next position of each particle.
    void oneStep(float h, float delta, glm::vec3 force);
};
