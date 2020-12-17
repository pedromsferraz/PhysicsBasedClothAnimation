#ifndef RENDERWIDGET_H
#define RENDERWIDGET_H

#include <QOpenGLWidget>
#include <QOpenGLExtraFunctions>
#include <QOpenGLShaderProgram>
#include <QVector3D>
#include <QMatrix4x4>
#include <QTimer>
#include <QTime>
#include <QElapsedTimer>
#include <vector>

#include "glm/glm.hpp"
#include "mesh/mesh.h"

class RenderWidget
        : public QOpenGLWidget
        , protected QOpenGLExtraFunctions
{
public:
    RenderWidget(QWidget* parent);
    virtual ~RenderWidget();

private:
    virtual void initializeGL();
    virtual void paintGL();
    virtual void resizeGL(int w, int h);

    virtual void mousePressEvent(QMouseEvent *event);
    virtual void mouseReleaseEvent(QMouseEvent *event);
    virtual void mouseMoveEvent(QMouseEvent *event);
    virtual void wheelEvent(QWheelEvent *event);

    // Timers and frame count for debugging
    QTimer timer;
    QElapsedTimer m_time;
    int m_frameCount = 0;

    // Camera information
    glm::vec3 eye;
    glm::vec3 center;
    glm::vec3 up;

    // Mesh
    Mesh mesh;

    // Arcball
    int radius;
    glm::ivec2 sphereCenter;
    glm::ivec2 oldPoint;
    bool moving;

    void initializeArcball();
    glm::quat pointToQuat(glm::ivec2 screenPoint);
    void rotate(glm::ivec2 p1, glm::ivec2 p2);

    // Mesh and vertex functions
    void createMesh();
    void updateMesh();
    void createVBO();
    void createTexture(const std::string& imagePath);

    QOpenGLShaderProgram program;

    unsigned int VAO;
    unsigned int VBO;
    unsigned int EBO;
    struct vertex
    {
        glm::vec3 pos;
        glm::vec3 normal;
//        glm::vec2 texCoord;
    };
    std::vector< vertex > vbo;
    std::vector< glm::vec3 > vertices;
    std::vector< glm::vec3 > normals;
    std::vector< glm::vec2 > texCoords;
    std::vector< unsigned int > indices;

    glm::mat4x4 model;
    glm::mat4x4 view;
    glm::mat4x4 proj;

    unsigned int textureID;
};

#endif // RENDERWIDGET_H
