#ifndef MYGL_H
#define MYGL_H

#include <glwidget277.h>
#include <utils.h>
#include <shaderprogram.h>
#include <scene/cylinder.h>
#include <scene/sphere.h>
#include <scene/mesh.h>
#include <scene/vertex.h>
#include <scene/joint.h>
#include <scene/drawvertex.h>
#include "camera.h"

#include <QOpenGLVertexArrayObject>
#include <QOpenGLShaderProgram>


class MyGL
    : public GLWidget277
{
    Q_OBJECT

private:
    Cylinder m_geomCylinder;// The instance of a unit cylinder we can use to render any cylinder
    Sphere m_geomSphere;// The instance of a unit sphere we can use to render any sphere
    Mesh m_geomMesh;
    ShaderProgram m_progLambert;// A shader program that uses lambertian reflection
    ShaderProgram m_progFlat;// A shader program that uses "flat" reflection (no shadowing at all)
    ShaderProgram prog_skeleton; // A shader program variable for manipulating skeleton

    GLuint vao; // A handle for our vertex array object. This will store the VBOs created in our geometry classes.
                // Don't worry too much about this. Just know it is necessary in order to render geometry.

    Camera m_glCamera;

    Vertex* currVert;
    HalfEdge* currHE;
    Face* currFace;

    drawVertex vertSelect;
    drawHE heSelect;
    drawFace faceSelect;
    std::vector<Joint*> skeleton;
    Joint* currJoint;

    bool skinPressed;

public:
    explicit MyGL(QWidget *parent = 0);
    ~MyGL();

    void setupCube();
    void setupTriangle();
    void initializeGL();
    void resizeGL(int w, int h);
    void paintGL();

    void setBindArray();
    void setJointTrans();

    // helper function for loading skeleton: sets children of a given node
    void setChildren(QJsonArray children, Joint* parent);

protected:
    void keyPressEvent(QKeyEvent *e);

signals:
    void sendFace(Face* face);
    void sendHE(HalfEdge* he);
    void sendVertex(Vertex* vert);
    void sendRoot(Joint* jt);
    void sendJointPos(glm::vec4 pos);


public slots:
    void slot_getCurrVertex(Vertex*);
    void slot_getCurrHE(HalfEdge*);
    void slot_getCurrFace(Face*);

    void slot_changeX(double);
    void slot_changeY(double);
    void slot_changeZ(double);

    void slot_changeR(double);
    void slot_changeG(double);
    void slot_changeB(double);

    void slot_addVertex(bool);
    void slot_triangulate(bool);
    void slot_subdivide(bool);
    void slot_extrude(bool);

    void slot_loadOBJ(bool);

    void slot_loadSkeleton(bool);
    void slot_storeCurrJoint(Joint*);

    // set joint influence of all vertices in mesh
    void slot_setVertexInfluence(bool pressed);

    void slot_changeJtX(double);
    void slot_changeJtY(double);
    void slot_changeJtZ(double);

    void slot_changeJtXRot(bool);
    void slot_changeJtYRot(bool);
    void slot_changeJtZRot(bool);

};


#endif // MYGL_H
