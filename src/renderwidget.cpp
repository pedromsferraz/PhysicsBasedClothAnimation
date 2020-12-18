#include "renderwidget.h"

#include <QImage>
#include <QMouseEvent>
#include <QGLWidget>
#include <glm/ext.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <cmath>

#ifndef M_PI
    #define M_PI 3.14159265358979323846
#endif

RenderWidget::RenderWidget(QWidget *parent)
    : QOpenGLWidget(parent),
      mesh(RectangularMesh(30, 20, 0.2f, 1.f, 20, 0.05, 0.02, glm::vec3(0.0f), glm::vec3(0.0f))),
      program(nullptr) {

    connect(&timer, SIGNAL(timeout()), this, SLOT(update()));
    if(format().swapInterval() == -1)
    {
        // V_blank synchronization not available (tearing likely to happen)
        qDebug("Swap Buffers at v_blank not available: refresh at approx 60fps.");
        timer.setInterval(17);
    }
    else
    {
        // V_blank synchronization available
        timer.setInterval(0);
    }
    timer.start();
}

RenderWidget::~RenderWidget()
{
    // Delete OpenGL resources
}

void RenderWidget::initializeGL()
{
    // Initialize OpenGL functions
    initializeOpenGLFunctions();

    // Define background color
    glClearColor(0.1f,0.1f,0.1f,1);

    // Define viewport
    glViewport(0, 0, width(), height());

    QString vertexPath = PROJECT_PATH;
    vertexPath += "vertexshader.glsl";

    QString fragmentPath = PROJECT_PATH;
    fragmentPath += "fragmentshader.glsl";

    // Compile shaders
    program.addShaderFromSourceFile(QOpenGLShader::Vertex, vertexPath);
    program.addShaderFromSourceFile(QOpenGLShader::Fragment, fragmentPath);
    program.link();

    // Configure camera
    this->eye = glm::vec3(4.0f, 4.0f, 5.0f);
    this->center = glm::vec3(0.0f, 0.0f, 0.0f);
    this->up = glm::vec3(0.0f, 1.0f, 0.0f);

    // Define view and projection matrices
    float ratio = static_cast<float>(width())/height();
    view = glm::lookAt(eye, center, up);
    proj = glm::perspective(glm::radians(45.0f), ratio, 0.1f, 100.0f);

    // Create mesh vertices, normals and texture
    createMesh();

    // Create VBO
    createVBO();

    // Initialize arcball
    initializeArcball();
    moving = true;
}

float mean_time = 0;
float mean_fps = 0;
void RenderWidget::paintGL()
{
    if (m_frameCount == 0) {
         m_time.start();
    } else {
        float time = m_time.elapsed() / (1000.0f * float(m_frameCount));
        float fps = 1.0 / time;
        mean_time += time;
        mean_fps += fps;
//        qDebug() << "Time: " << time << " - FPS: " << fps;
        if (m_frameCount == 1000)
            qDebug() << "Mean time: " << mean_time / float(m_frameCount) << " - Mean FPS: " << mean_fps / float(m_frameCount);
    }
    m_frameCount++;

    // Enable Z test
    glEnable(GL_DEPTH_TEST);

    // Clear screen
    glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);

    // Link VAO
    glBindVertexArray(VAO);

    // Bind program e pass the following uniforms:
    // Matrices
    // Light position
    // Ambient, diffuse, specular e shininess components of the material
    program.bind();

    QMatrix4x4 m(glm::value_ptr(glm::transpose(model)));
    QMatrix4x4 v(glm::value_ptr(glm::transpose(view)));
    QMatrix4x4 p(glm::value_ptr(glm::transpose(proj)));

    // Pass light and material uniforms
    program.setUniformValue("light.position", v*QVector3D(0,-1,-5) );
    program.setUniformValue("material.ambient", QVector3D(0.15f,0.15f,0.15f));
    program.setUniformValue("material.diffuse", QVector3D(1.0f,0.5f,1.0f));
    program.setUniformValue("material.specular", QVector3D(1.0f,1.0f,1.0f));
    program.setUniformValue("material.shininess", 24.0f);

    // Activate and bind texture
//    glActiveTexture(GL_TEXTURE0);
//    glBindTexture(GL_TEXTURE_2D, textureID);
//    program.setUniformValue("sampler", 0);

    // Scale matrix for resizing mesh
    QMatrix4x4 scale(glm::value_ptr(glm::scale(glm::vec3(.2f))));

    // Pass mv and mvp matrices
    // QMatrix4x4 mv = v * m * sphereModel;
    QMatrix4x4 mv = v * scale * m;
    QMatrix4x4 mvp = p * mv;

    program.setUniformValue("mv", mv);
    program.setUniformValue("mv_ti", mv.inverted().transposed());
    program.setUniformValue("mvp", mvp);

    // Update mesh and draw
    updateMesh();
//    glPolygonMode( GL_FRONT_AND_BACK, GL_LINE );
    glDrawElements(GL_TRIANGLES, static_cast<int>(indices.size()), GL_UNSIGNED_INT, nullptr);
}

void RenderWidget::resizeGL(int w, int h)
{
    // Update viewport
    glViewport(0, 0, w, h);

    // Update projection matrix
    float ratio = static_cast<float>(w)/h;
    proj = glm::perspective(glm::radians(45.0f), ratio, 0.1f, 100.0f);

    initializeArcball();
}

void RenderWidget::mousePressEvent(QMouseEvent *event)
{
    oldPoint = {event->x(), event->y()};
    moving = true;
}

void RenderWidget::mouseReleaseEvent(QMouseEvent *event)
{
    moving = false;
}

void RenderWidget::mouseMoveEvent(QMouseEvent *event)
{
    if (!moving)
        return;
    glm::ivec2 newPoint = {event->x(), event->y()};
    rotate(oldPoint, newPoint);
    oldPoint = newPoint;
    update();
}

void RenderWidget::wheelEvent(QWheelEvent *event)
{
    eye += (center - eye) * 0.001f * event->delta();
    view = glm::lookAt(eye, center, up);
    update();
}

void RenderWidget::initializeArcball()
{
    int w = width();
    int h = height();

    radius = std::max(w, h);
    sphereCenter = {w * 0.5f, h * 0.5f};
    moving = false;
}

glm::quat RenderWidget::pointToQuat(glm::ivec2 screenPoint)
{
    if (radius > 0) {
        glm::vec3 v(glm::vec2(screenPoint - sphereCenter)/radius, 0.0f);

        auto r = glm::length2(v);
        if (r > 1)
            v *= (1.0f / glm::sqrt(r));
        else
            v.z = glm::sqrt(1.0f-r);

        return glm::quat(0.0f, v);
    }

    return glm::quat();
}

void RenderWidget::rotate(glm::ivec2 p1, glm::ivec2 p2)
{
    int h = height();
    auto q1 = pointToQuat({p1.x, h - p1.y});
    auto q2 = pointToQuat({p2.x, h - p2.y});
    auto q = q2 * glm::conjugate(q1);

    auto R = glm::mat4_cast(q);
    model = R * model;
}

void RenderWidget::createMesh()
{
    int n = mesh.n, m = mesh.m;
    for (int i = 0; i < n; ++i) {
        for (int j = 0; j < m; ++j) {
            vertices.push_back(mesh.particles[i][j].position);
        }
    }

    for (int i = 0; i < n; ++i) {
        for (int j = 0; j < m; ++j) {
            normals.push_back(glm::vec3(0.0f, 0.0f, 1.0f));
        }
    }

    for (int i = 0; i < n-1; ++i) {
        for (int j = 0; j < m-1; ++j) {
            int k = i*m + j;
            indices.push_back(k);
            indices.push_back(k+m+1);
            indices.push_back(k+m);

            indices.push_back(k);
            indices.push_back(k+1);
            indices.push_back(k+m+1);
        }
    }
}

void RenderWidget::updateMesh()
{
    QTime time = QTime::currentTime();
    float sin_t = glm::sin(0.5*time.msecsSinceStartOfDay());

    glm::vec3 gravity = glm::vec3(0.0f, -9.8f, 0.0f);
    glm::vec3 wind = glm::vec3(5.0f + 2.0f*sin_t, 6.0f + 6.0f*sin_t, -2.0f+sin_t);
    mesh.setForce(gravity+wind);
    mesh.oneStep();

    int n = mesh.n, m = mesh.m;
    for (int i = 0; i < n; ++i) {
        for (int j = 0; j < m; ++j) {
            int k = i*m + j;
            vbo[k].pos = mesh.particles[i][j].position;
            vbo[k].normal = glm::vec3(0.0f);
        }
    }

    for (int i = 0; i < n-1; ++i) {
        for (int j = 0; j < m-1; ++j) {
            int k = i*m + j;
            glm::vec3 v1 = vbo[k+1].pos - vbo[k].pos;
            glm::vec3 v2 = vbo[k+m+1].pos - vbo[k].pos;
            glm::vec3 n1 = glm::cross(v1, v2);
            vbo[k].normal += n1;
            vbo[k+1].normal += n1;
            vbo[k+m+1].normal += n1;

            glm::vec3 v3 = vbo[k+m].pos - vbo[k].pos;
            glm::vec3 n2 = glm::cross(v2, v3);
            vbo[k].normal += n2;
            vbo[k+m+1].normal += n2;
            vbo[k+m].normal += n2;
        }
    }

    for (int i = 0; i < n; ++i) {
        for (int j = 0; j < m; ++j) {
            int k = i*m + j;
            vbo[k].normal = glm::normalize(vbo[k].normal);
        }
    }

    void *ptr = glMapBuffer(GL_ARRAY_BUFFER, GL_WRITE_ONLY);
    memcpy(ptr, &vbo[0], vbo.size() * sizeof(vertex));
    glUnmapBuffer(GL_ARRAY_BUFFER);
}

void RenderWidget::createVBO()
{
    // Construct VBO vector

    vbo.reserve( vertices.size() );
    for( unsigned int i = 0; i < vertices.size(); i++ )
    {
//        vbo.push_back({vertices[i], normals[i], texCoords[i]});
        vbo.push_back({vertices[i], normals[i]});
    }

    // Create and bind VBO and copy data
    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, vbo.size() * sizeof(vertex), &vbo[0], GL_DYNAMIC_DRAW);

    // Create and bind EBO and copy data
    glGenBuffers(1, &EBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), &indices[0], GL_DYNAMIC_DRAW);

    // Create and bind EBO and define layouts
    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);

    // Enable, bind and define buffer layout
    glBindBuffer(GL_ARRAY_BUFFER, VBO);

    glEnableVertexAttribArray( 0 );
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(vertex), (void*) 0 );

    glEnableVertexAttribArray( 1 );
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(vertex), (void*) sizeof(glm::vec3) );

//    glEnableVertexAttribArray( 2 );
//    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(vertex), (void*) (2*sizeof(glm::vec3)) );

    // Bind EBO
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
}


void RenderWidget::createTexture(const std::string& imagePath)
{
    // Create texture
    glGenTextures(1, &textureID);
    
    // Bind created texture
    glBindTexture(GL_TEXTURE_2D, textureID);
    
    // Open image file with QT
    QImage texImage = QGLWidget::convertToGLFormat(QImage(imagePath.c_str()));

    // Send image to OpenGL
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, texImage.width(), texImage.height(), 0, GL_RGBA,GL_UNSIGNED_BYTE, texImage.bits());

    // Define filter parameters and generate mipmap
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);

    glGenerateMipmap(GL_TEXTURE_2D);
}
