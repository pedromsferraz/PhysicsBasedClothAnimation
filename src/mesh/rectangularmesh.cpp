#include <glm/glm.hpp>
#include <iostream>
#include <vector>
#include "rectangularmesh.h"

using edge = std::pair<std::pair<int, int>, std::pair<int, int> >;

bool RectangularMesh::inBounds(int n, int m, int i, int j) {
    return i >= 0 && i < n && j >= 0 && j < m;
}

void RectangularMesh::createBarIfNotExist(int n, int m, int i, int j, int k, int l) {
    edge e = {{i, j}, {k, l}};
    if (inBounds(n, m, k, l) && edges.find(e) == edges.end()) {
        float distance = glm::length(particles[i][j].position - particles[k][l].position);
        Bar bar = Bar(particles[i][j], particles[k][l], distance);
        this->bars.push_back(bar);
        edges.insert(e);
    }
}

RectangularMesh::RectangularMesh(int n, int m,
                                 float mass,
                                 float barLength,
                                 int n_relaxations,
                                 float h,
                                 float delta,
                                 glm::vec3 force = glm::vec3(0.0f),
                                 glm::vec3 initialVelocity = glm::vec3(0.0f)) {
    Particle p = Particle(mass, glm::vec3(0.0f), false);
    particles.assign(n, std::vector<Particle>(m, p));
    this->force = force;
    this->n = n;
    this->m = m;
    this->n_relaxations = n_relaxations;
    this->h = h;
    this->delta = delta;
    this->force = force;

    glm::vec3 initialPosition = glm::vec3(-(0.5f * (n-1.0f)) * (barLength), -(0.5f * (m-1.0f)) * (barLength), 0.0f);

    particles[0][m-1].isFixed = true;
    particles[n-1][m-1].isFixed = true;
    for (int i = 0; i < n; ++i) {
        for (int j = 0; j < m; ++j) {
            particles[i][j].previousPosition = initialPosition + glm::vec3((1.0f * i) * (barLength),
                                                                           (1.0f * j) * (barLength),
                                                                           0.0f );
            if (particles[i][j].isFixed) {
                particles[i][j].position = particles[i][j].previousPosition;
            } else {
                particles[i][j].position = particles[i][j].previousPosition + h*initialVelocity;
            }
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

void RectangularMesh::oneStep(float h, float delta, glm::vec3 force) {
    for (int i = 0; i < n; ++i) {
        for (int j = 0; j < m; ++j) {
            if (particles[i][j].isFixed)
                continue;
            glm::vec3 pos = particles[i][j].position;
            glm::vec3 prevPos = particles[i][j].previousPosition;
            float mass = particles[i][j].mass;
            pos = pos + (1.0f - delta) * (pos - prevPos) + ((h*h) / mass) * force;
            particles[i][j].previousPosition = particles[i][j].position;
            particles[i][j].position = pos;
        }
    }

    for (int i = 0; i < n_relaxations; ++i) {
        for (auto &bar : bars) {
            bar.update();
        }
    }
}

void RectangularMesh::oneStep() {
    oneStep(this->h, this->delta, this->force);
}
