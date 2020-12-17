#include <glm/glm.hpp>
#include <iostream>
#include <vector>
#include <set>
#include "mesh.h"

using edge = std::pair<std::pair<int, int>, std::pair<int, int> >;

bool Mesh::inBounds(int n, int m, int i, int j) {
    return i >= 0 && i < n && j >= 0 && j < m;
}

void Mesh::createBarIfNotExist(int n, int m, int i, int j, int k, int l) {
    edge e = {{i, j}, {k, l}};
    if (inBounds(n, m, k, l) && edges.find(e) == edges.end()) {
        float distance = glm::length(particles[i][j].position - particles[k][l].position);
        Bar bar = Bar(particles[i][j], particles[k][l], distance, tolerance);
        this->bars.push_back(bar);
        edges.insert(e);
    }
}

Mesh::Mesh(int n, int m, float mass, float barLength) {
    Particle p = Particle(mass, glm::vec3(0.0f), false);
    particles.assign(n, std::vector<Particle>(m, p));
    previousParticles.assign(n, std::vector<Particle>(m, p));
    this->force = glm::vec3(0.0f);
    this->n = n;
    this->m = m;

    glm::vec3 initialPosition = glm::vec3(-(0.5f * (n-1.0f)) * (barLength), -(0.5f * (m-1.0f)) * (barLength), 0.0f);

    for (int i = 0; i < n; ++i) {
        for (int j = 0; j < m; ++j) {
            particles[0][j].isFixed = true;
            previousParticles[0][j].isFixed = true;
            particles[i][j].position = initialPosition + glm::vec3((1.0f * i) * (barLength), 
                                                                    (1.0f * j) * (barLength), 
                                                                     0.0f );
            previousParticles[i][j].position = particles[i][j].position;
        }
    }

    for (int i = 0; i < n; ++i) {
        for (int j = 0; j < m; ++j) {
            createBarIfNotExist(n, m, i, j, i-1, j); // north
            createBarIfNotExist(n, m, i, j, i-1, j+1); // northeast
            createBarIfNotExist(n, m, i, j, i, j+1); // east
            createBarIfNotExist(n, m, i, j, i+1, j+1); // southeast
            createBarIfNotExist(n, m, i, j, i+1, j); // south
            createBarIfNotExist(n, m, i, j, i+1, j-1); // southwest
            createBarIfNotExist(n, m, i, j, i, j-1); // west
            createBarIfNotExist(n, m, i, j, i-1, j-1); // west

            createBarIfNotExist(n, m, i, j, i-2, j); // north
            createBarIfNotExist(n, m, i, j, i-2, j+2); // northeast
            createBarIfNotExist(n, m, i, j, i, j+2); // east
            createBarIfNotExist(n, m, i, j, i+2, j+2); // southeast
            createBarIfNotExist(n, m, i, j, i+2, j); // south
            createBarIfNotExist(n, m, i, j, i+2, j-2); // southwest
            createBarIfNotExist(n, m, i, j, i, j-2); // west
            createBarIfNotExist(n, m, i, j, i-2, j-2); // west
        }
    }
}

void Mesh::setForce(glm::vec3 force) {
    this->force = force;
}

void Mesh::oneStep(float h, float delta, glm::vec3 force) {
    bool isDone = false;

    for (int i = 0; i < n; ++i) {
        for (int j = 0; j < m; ++j) {
            if (particles[i][j].isFixed)
                continue;
            glm::vec3 pos = particles[i][j].position;
            glm::vec3 prevPos = previousParticles[i][j].position;
            float mass = particles[i][j].mass;
            pos = pos + (1.0f - delta) * (pos - prevPos) + ((h*h) / mass) * force;
            particles[i][j].position = pos;
        }
    }

    for (int i = 0; !isDone && i < 20; ++i) {
        isDone = true;
        for (auto &bar : bars) {
            bool isRelaxed = bar.update();
            isDone = isDone && isRelaxed;
        }
    }
    previousParticles = particles;
}

void Mesh::oneStep(float h, float delta) {
    oneStep(h, delta, this->force);
}

void Mesh::simulate() {
    float h = 1e-2;
    float delta = 0.02;

    for (int i = 0; i < 1000; ++i) {
        oneStep(h, delta);
    }
}
