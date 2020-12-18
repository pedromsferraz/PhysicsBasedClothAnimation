#include <glm/glm.hpp>
#include <vector>
#include "mesh.h"

class GenericMesh : public Mesh {

    void DFSUtil(int u, std::vector<std::vector<int> > &adj, std::vector<Particle> &particles, std::vector<bool> &visited);
    void DFS(std::vector<std::vector<int> > &adj, std::vector<Particle> &particles);

public:

    std::vector<Particle> particles;

    // Generic mesh constructor
    GenericMesh(std::vector<std::vector<int> > &meshGraph,
                std::vector<Particle> &particle_list,
                int n_relaxations,
                float h,
                float delta,
                glm::vec3 force,
                glm::vec3 initialVelocity);

    // Implementation of oneStep without receiving paramenters.
    void oneStep();

    // Receives the step, the damping coefficient and the force that acts on the mesh
    // and calculates the next position of each particle.
    void oneStep(float h, float delta, glm::vec3 force);
};
