#include <glm/glm.hpp>
#include <vector>
#include "mesh.h"

class GenericMesh : public Mesh {

    void DFSUtil(int u, std::vector<std::vector<int> > &adj, std::vector<Particle> &particles, std::vector<bool> &visited);
    void DFS(std::vector<std::vector<int> > &adj, std::vector<Particle> &particles);

public:

    std::vector<Particle> particles;

    GenericMesh(std::vector<std::vector<int> > &meshGraph,
                std::vector<Particle> &particle_list,
                int n_relaxations,
                float h,
                float delta,
                glm::vec3 force,
                glm::vec3 initialVelocity);
    void oneStep();
    void oneStep(float h, float delta, glm::vec3 force);
};
