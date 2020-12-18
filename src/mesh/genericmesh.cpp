#include "genericmesh.h"
#include <iostream>
#include <vector>

GenericMesh::GenericMesh(std::vector<std::vector<int> > &meshGraph,
                         std::vector<Particle> &particle_list,
                         int n_relaxations,
                         float h,
                         float delta,
                         glm::vec3 force = glm::vec3(0.0f),
                         glm::vec3 initialVelocity = glm::vec3(0.0f)) {
    this->force = force;
    this->n_relaxations = n_relaxations;
    this->h = h;
    this->delta = delta;
    this->force = force;
    DFS(meshGraph, particle_list);

    for (auto p : particle_list) {
        if (!p.isFixed) {
            p.previousPosition = p.position;
            p.position = p.previousPosition + h * initialVelocity;
        }
        this->particles.push_back(p);
    }
}

// A utility function to do DFS of graph recursively from a given vertex u.
void GenericMesh::DFSUtil(int u, std::vector<std::vector<int> > &adj, std::vector<Particle> &particles, std::vector<bool> &visited) {
    visited[u] = true;
    for (auto v : adj[u]) {
        if (!visited[v]) {
            float distance = glm::length(particles[u].position - particles[v].position);
            Bar bar = Bar(particles[u], particles[v], distance);
            this->bars.push_back(bar);

            DFSUtil(v, adj, particles, visited);
        }
    }
}

// This function does DFSUtil() for all unvisited vertices.
void GenericMesh::DFS(std::vector<std::vector<int> > &adj, std::vector<Particle> &particles) {
    std::vector<bool> visited(adj.size(), false);
    for (int v = 0; v < adj.size(); v++)
        if (visited[v] == false)
            DFSUtil(v, adj, particles, visited);
}

void GenericMesh::oneStep(float h, float delta, glm::vec3 force) {
    for (Particle particle : particles) {
        if (particle.isFixed)
            continue;
        glm::vec3 pos = particle.position;
        glm::vec3 prevPos = particle.previousPosition;
        float mass = particle.mass;
        pos = pos + (1.0f - delta) * (pos - prevPos) + ((h*h) / mass) * force;
        particle.previousPosition = particle.position;
        particle.position = pos;
    }

    for (int i = 0; i < n_relaxations; ++i) {
        for (auto &bar : bars) {
            bar.update();
        }
    }
}

void GenericMesh::oneStep() {
    oneStep(this->h, this->delta, this->force);
}
