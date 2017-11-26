#include "mygl.h"
#include <la.h>

#include <iostream>
#include <QApplication>
#include <QKeyEvent>
#include <QFileDialog>
#include <QString>
#include <QJsonDocument>
#include <QJsonArray>
#include <QJsonObject>
#include <QJsonValue>
#include <glm/gtc/matrix_transform.hpp>


MyGL::MyGL(QWidget *parent)
    : GLWidget277(parent),
      m_geomCylinder(this), m_geomSphere(this),
      m_geomMesh(this),
      m_progLambert(this), m_progFlat(this), prog_skeleton(this),
      m_glCamera(),
      currVert(nullptr),
      currHE(nullptr),
      currFace(nullptr),
      vertSelect(this),
      heSelect(this),
      faceSelect(this),
      skeleton(std::vector<Joint*>()),
      currJoint(nullptr),
      skinPressed(false)
{
    setFocusPolicy(Qt::StrongFocus);
}

MyGL::~MyGL()
{
    makeCurrent();
    glDeleteVertexArrays(1, &vao);
    m_geomCylinder.destroy();
    m_geomSphere.destroy();
    m_geomMesh.destroy();
    vertSelect.destroy();
    heSelect.destroy();
    faceSelect.destroy();
    for (Joint* jt : skeleton) {
        jt->destroy();
    }
}

void MyGL::setupCube() {
    // create vertices
    std::vector<Vertex*> verts;
    verts.push_back(new Vertex(glm::vec4(-0.5, -0.5, 0.5, 1)));    // 0
    verts.push_back(new Vertex(glm::vec4(0.5, -0.5, 0.5, 1)));     // 1
    verts.push_back(new Vertex(glm::vec4(0.5, 0.5, 0.5, 1)));      // 2
    verts.push_back(new Vertex(glm::vec4(-0.5, 0.5, 0.5, 1)));     // 3
    verts.push_back(new Vertex(glm::vec4(-0.5, -0.5, -0.5, 1)));   // 4
    verts.push_back(new Vertex(glm::vec4(-0.5, 0.5, -0.5, 1)));    // 5
    verts.push_back(new Vertex(glm::vec4(0.5, 0.5, -0.5, 1)));     // 6
    verts.push_back(new Vertex(glm::vec4(0.5, -0.5, -0.5, 1)));    // 7

    std::vector<Face*> faces;

    faces.push_back(new Face(glm::vec4(1, 0, 0, 1))); // 0: red
    faces.push_back(new Face(glm::vec4(0, 1, 0, 1))); // 1: green
    faces.push_back(new Face(glm::vec4(0, 0, 1, 1))); // 2: blue
    faces.push_back(new Face(glm::vec4(1, 1, 0, 1))); // 3: yellow
    faces.push_back(new Face(glm::vec4(0, 1, 1, 1))); // 4: cyan
    faces.push_back(new Face(glm::vec4(1, 0, 1, 1))); // 5: magenta

    // create half-edges
    std::vector<HalfEdge*> halfedges;
    // face 0
    halfedges.push_back(new HalfEdge(faces.at(0), verts.at(0)));    // 0
    halfedges.push_back(new HalfEdge(faces.at(0), verts.at(1)));    // 1
    halfedges.push_back(new HalfEdge(faces.at(0), verts.at(2)));    // 2
    halfedges.push_back(new HalfEdge(faces.at(0), verts.at(3)));    // 3

    // face 1
    halfedges.push_back(new HalfEdge(faces.at(1), verts.at(1)));    // 4
    halfedges.push_back(new HalfEdge(faces.at(1), verts.at(7)));    // 5
    halfedges.push_back(new HalfEdge(faces.at(1), verts.at(6)));    // 6
    halfedges.push_back(new HalfEdge(faces.at(1), verts.at(2)));    // 7

    // face 2
    halfedges.push_back(new HalfEdge(faces.at(2), verts.at(7)));    // 8
    halfedges.push_back(new HalfEdge(faces.at(2), verts.at(4)));    // 9
    halfedges.push_back(new HalfEdge(faces.at(2), verts.at(5)));    // 10
    halfedges.push_back(new HalfEdge(faces.at(2), verts.at(6)));    // 11

    // face 3
    halfedges.push_back(new HalfEdge(faces.at(3), verts.at(4)));    // 12
    halfedges.push_back(new HalfEdge(faces.at(3), verts.at(0)));    // 13
    halfedges.push_back(new HalfEdge(faces.at(3), verts.at(3)));    // 14
    halfedges.push_back(new HalfEdge(faces.at(3), verts.at(5)));    // 15

    // face 4
    halfedges.push_back(new HalfEdge(faces.at(4), verts.at(0)));    // 16
    halfedges.push_back(new HalfEdge(faces.at(4), verts.at(4)));    // 17
    halfedges.push_back(new HalfEdge(faces.at(4), verts.at(7)));    // 18
    halfedges.push_back(new HalfEdge(faces.at(4), verts.at(1)));    // 19

    // face 5
    halfedges.push_back(new HalfEdge(faces.at(5), verts.at(2)));    // 20
    halfedges.push_back(new HalfEdge(faces.at(5), verts.at(6)));    // 21
    halfedges.push_back(new HalfEdge(faces.at(5), verts.at(5)));    // 22
    halfedges.push_back(new HalfEdge(faces.at(5), verts.at(3)));    // 23

    for (int i = 0; i < faces.size(); i++) {
        faces.at(i)->setHE(halfedges.at(4 * i));
    }

    // set pointers to HEs for vertices
    verts.at(0)->setEdge(halfedges.at(0));
    verts.at(1)->setEdge(halfedges.at(1));
    verts.at(2)->setEdge(halfedges.at(2));
    verts.at(3)->setEdge(halfedges.at(3));
    verts.at(4)->setEdge(halfedges.at(9));
    verts.at(5)->setEdge(halfedges.at(10));
    verts.at(6)->setEdge(halfedges.at(11));
    verts.at(7)->setEdge(halfedges.at(8));

    // set next pointers
    for (HalfEdge* he : halfedges) {
        int id = he->getID();
        if (id % 4 != 3) {
            he->setNextHE(halfedges.at(id + 1));
        } else {
            he->setNextHE(halfedges.at(id - 3));
        }
    }

    // set sym pointers
    // face 0
    halfedges.at(0)->setSymHE(halfedges.at(14));
    halfedges.at(1)->setSymHE(halfedges.at(16));
    halfedges.at(2)->setSymHE(halfedges.at(4));
    halfedges.at(3)->setSymHE(halfedges.at(20));

    // face 1
    halfedges.at(4)->setSymHE(halfedges.at(2));
    halfedges.at(5)->setSymHE(halfedges.at(19));
    halfedges.at(6)->setSymHE(halfedges.at(8));
    halfedges.at(7)->setSymHE(halfedges.at(21));

    // face 2
    halfedges.at(8)->setSymHE(halfedges.at(6));
    halfedges.at(9)->setSymHE(halfedges.at(18));
    halfedges.at(10)->setSymHE(halfedges.at(12));
    halfedges.at(11)->setSymHE(halfedges.at(22));

    // face 3
    halfedges.at(12)->setSymHE(halfedges.at(10));
    halfedges.at(13)->setSymHE(halfedges.at(17));
    halfedges.at(14)->setSymHE(halfedges.at(0));
    halfedges.at(15)->setSymHE(halfedges.at(23));

    // face 4
    halfedges.at(16)->setSymHE(halfedges.at(1));
    halfedges.at(17)->setSymHE(halfedges.at(13));
    halfedges.at(18)->setSymHE(halfedges.at(9));
    halfedges.at(19)->setSymHE(halfedges.at(5));

    // face 5
    halfedges.at(20)->setSymHE(halfedges.at(3));
    halfedges.at(21)->setSymHE(halfedges.at(7));
    halfedges.at(22)->setSymHE(halfedges.at(11));
    halfedges.at(23)->setSymHE(halfedges.at(15));

    // create cube mesh and set values
    m_geomMesh.setVerts(verts);
    m_geomMesh.setFaces(faces);
    m_geomMesh.setHEs(halfedges);

}


void MyGL::initializeGL()
{
    // Create an OpenGL context using Qt's QOpenGLFunctions_3_2_Core class
    // If you were programming in a non-Qt context you might use GLEW (GL Extension Wrangler)instead
    initializeOpenGLFunctions();
    // Print out some information about the current OpenGL context
    debugContextVersion();

    // Set a few settings/modes in OpenGL rendering
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_LINE_SMOOTH);
    glEnable(GL_POLYGON_SMOOTH);
    glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);
    glHint(GL_POLYGON_SMOOTH_HINT, GL_NICEST);
    // Set the size with which points should be rendered
    glPointSize(5);
    // Set the color with which the screen is filled at the start of each render call.
    glClearColor(0.5, 0.5, 0.5, 1);

    printGLErrorLog();

    // Create a Vertex Attribute Object
    glGenVertexArrays(1, &vao);

    // Create cube mesh
    setupCube();
    m_geomMesh.create();

    // Create and set up the diffuse shader
    m_progLambert.create(":/glsl/lambert.vert.glsl", ":/glsl/lambert.frag.glsl");
    // Create and set up the flat lighting shader
    m_progFlat.create(":/glsl/flat.vert.glsl", ":/glsl/flat.frag.glsl");
    prog_skeleton.create(":/glsl/skeleton.vert.glsl", ":/glsl/skeleton.frag.glsl");

    // Set a color with which to draw geometry since you won't have one
    // defined until you implement the Node classes.
    // This makes your geometry render green.
    m_progLambert.setGeometryColor(glm::vec4(0,1,0,1));

    // We have to have a VAO bound in OpenGL 3.2 Core. But if we're not
    // using multiple VAOs, we can just bind one once.
    //    vao.bind();
    glBindVertexArray(vao);


    for (Vertex* v : m_geomMesh.getVerts()) {
        emit sendVertex(v);
    }

    for (Face* f : m_geomMesh.getFaces()) {
        emit sendFace(f);
    }

    for (HalfEdge* he : m_geomMesh.getHEs()) {
        emit sendHE(he);
    }
}

void MyGL::resizeGL(int w, int h)
{
    //This code sets the concatenated view and perspective projection matrices used for
    //our scene's camera view.
    m_glCamera = Camera(w, h);
    glm::mat4 viewproj = m_glCamera.getViewProj();

    // Upload the view-projection matrix to our shaders (i.e. onto the graphics card)

    m_progLambert.setViewProjMatrix(viewproj);
    m_progFlat.setViewProjMatrix(viewproj);
    prog_skeleton.setViewProjMatrix(viewproj);

    printGLErrorLog();
}

//This function is called by Qt any time your GL window is supposed to update
//For example, when the function updateGL is called, paintGL is called implicitly.
//DO NOT CONSTRUCT YOUR SCENE GRAPH IN THIS FUNCTION!
void MyGL::paintGL()
{
    // Clear the screen so that we only see newly drawn images
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // hides errors
    glGetError();

    m_progFlat.setViewProjMatrix(m_glCamera.getViewProj());
    m_progLambert.setViewProjMatrix(m_glCamera.getViewProj());
    prog_skeleton.setViewProjMatrix(m_glCamera.getViewProj());

    //#define NOPE
#ifndef NOPE
    glm::mat4 model;

    if (skinPressed) {
        prog_skeleton.setModelMatrix(model);
        prog_skeleton.draw(m_geomMesh);
    } else {
        m_progLambert.setModelMatrix(model);
        m_progLambert.draw(m_geomMesh);
    }

    // disable depth when drawing mesh components
    glDisable(GL_DEPTH_TEST);

    // draw skeleton
    m_progFlat.setModelMatrix(model);
    for (Joint* bone : skeleton) {
         m_progFlat.draw(*bone);
    }

    // draw selected mesh component

    m_progFlat.setModelMatrix(model);
    m_progFlat.draw(vertSelect);

    m_progFlat.setModelMatrix(model);
    m_progFlat.draw(faceSelect);

    m_progFlat.setModelMatrix(model);
    m_progFlat.draw(heSelect);

    // enable it again!
    glEnable(GL_DEPTH_TEST);

#endif
}


void MyGL::keyPressEvent(QKeyEvent *e)
{

    float amount = 2.0f;
    if(e->modifiers() & Qt::ShiftModifier){
        amount = 10.0f;
    }
    // http://doc.qt.io/qt-5/qt.html#Key-enum
    // This could all be much more efficient if a switch
    // statement were used, but I really dislike their
    // syntax so I chose to be lazy and use a long
    // chain of if statements instead
    if (e->key() == Qt::Key_Escape) {
        QApplication::quit();
    } else if (e->key() == Qt::Key_Right) {
        m_glCamera.RotatePhi(-amount);
    } else if (e->key() == Qt::Key_Left) {
        m_glCamera.RotatePhi(amount);
    } else if (e->key() == Qt::Key_Up) {
        m_glCamera.RotateTheta(-amount);
    } else if (e->key() == Qt::Key_Down) {
        m_glCamera.RotateTheta(amount);
    } else if (e->key() == Qt::Key_1) {
        m_glCamera.Zoom(amount);
    } else if (e->key() == Qt::Key_2) {
        m_glCamera.fovy -= amount;
    } else if (e->key() == Qt::Key_W) {
        m_glCamera.Zoom(amount);
    } else if (e->key() == Qt::Key_S) {
        m_glCamera.Zoom(-amount);
    } else if (e->key() == Qt::Key_D) {
        m_glCamera.TranslateAlongRight(amount);
    } else if (e->key() == Qt::Key_A) {
        m_glCamera.TranslateAlongRight(-amount);
    } else if (e->key() == Qt::Key_Q) {
        m_glCamera.TranslateAlongUp(-amount);
    } else if (e->key() == Qt::Key_E) {
        m_glCamera.TranslateAlongUp(amount);
    } else if (e->key() == Qt::Key_R) {
        m_glCamera = Camera(this->width(), this->height());

        // visual debugging tools
    } else if (e->key() == Qt::Key_N) {
        if (currHE != nullptr) {
            currHE = currHE->getNextHE();
            heSelect.setHE(currHE);
            heSelect.create();
        }
    } else if (e->key() == Qt::Key_M) {
        if (currHE != nullptr) {
            currHE = currHE->getSymHE();
            heSelect.setHE(currHE);
            heSelect.create();
        }
    } else if (e->key() == Qt::Key_F) {
        if (currHE != nullptr) {
            currFace = currHE->getFace();
            faceSelect.setFace(currFace);
            faceSelect.create();
        }
    } else if (e->key() == Qt::Key_V) {
        if (currHE != nullptr) {
            currVert = currHE->getVert();
            vertSelect.setVert(currVert);
            vertSelect.create();
        }
    } else if (e->key() == Qt::Key_H) {
        if (currVert != nullptr) {
            currHE = currVert->getEdge();
            heSelect.setHE(currHE);
            heSelect.create();
        }
    } else if (e->key() == Qt::Key_J) {
        if (currFace != nullptr) {
            currHE = currFace->getHE();
            heSelect.setHE(currHE);
            heSelect.create();
        }
    }
    m_glCamera.RecomputeAttributes();
    update();  // Calls paintGL, among other things
}


// set current mesh components

void MyGL::slot_getCurrVertex(Vertex* v) {
    currVert = v;
    vertSelect.setVert(currVert);
    vertSelect.create();
    update();
}

void MyGL::slot_getCurrHE(HalfEdge* he) {
    currHE = he;
    heSelect.setHE(currHE);
    heSelect.create();
    update();
}

void MyGL::slot_getCurrFace(Face* f) {
    currFace = f;
    faceSelect.setFace(currFace);
    faceSelect.create();
    update();
}

// change vertex position with spin boxes

void MyGL::slot_changeX(double x) {
    if (currVert != nullptr) {
        glm::vec4 coord = currVert->getCoord();
        currVert->setCoord(glm::vec4(coord[0] + x, coord[1], coord[2], 1));
        m_geomMesh.destroy();
        m_geomMesh.create();
        update();
    }
}

void MyGL::slot_changeY(double y) {
    if (currVert != nullptr) {
        glm::vec4 coord = currVert->getCoord();
        currVert->setCoord(glm::vec4(coord[0], coord[1] + y, coord[2], 1));
        m_geomMesh.destroy();
        m_geomMesh.create();
        update();
    }
}

void MyGL::slot_changeZ(double z) {
    if (currVert != nullptr) {
        glm::vec4 coord = currVert->getCoord();
        currVert->setCoord(glm::vec4(coord[0], coord[1], coord[2] + z, 1));
        m_geomMesh.destroy();
        m_geomMesh.create();
        update();
    }
}

// change face color with spin boxes

void MyGL::slot_changeR(double r) {
    if (currFace != nullptr) {
        glm::vec4 col = currFace->getColor();
        currFace->setColor(glm::vec4(r, col[1], col[2], 1));
        m_geomMesh.destroy();
        m_geomMesh.create();
        update();
    }
}

void MyGL::slot_changeG(double g){
    if (currFace != nullptr) {
        glm::vec4 col = currFace->getColor();
        currFace->setColor(glm::vec4(col[0], g, col[2], 1));
        m_geomMesh.destroy();
        m_geomMesh.create();
        update();
    }
}

void MyGL::slot_changeB(double b){
    if (currFace != nullptr) {
        glm::vec4 col = currFace->getColor();
        currFace->setColor(glm::vec4(col[0], col[1], b, 1));
        m_geomMesh.destroy();
        m_geomMesh.create();
        update();
    }
}

// add vertex to current HE
void MyGL::slot_addVertex(bool pressed) {
    if (pressed && currHE != nullptr) {
        m_geomMesh.splitEdge(currHE);
        m_geomMesh.destroy();
        m_geomMesh.create();

        // send name of the new vertex / half-edge
        std::vector<Vertex*> verts = m_geomMesh.getVerts();
        sendVertex(verts.at(verts.size() - 1));
        std::vector<HalfEdge*> hes = m_geomMesh.getHEs();
        sendHE(hes.at(hes.size() - 1));

        update();
    }
}

// triangulate current face
void MyGL::slot_triangulate(bool pressed) {
    if (pressed && currFace != nullptr) {
        std::vector<Face*> newFaces;
        std::vector<HalfEdge*> newHEs;
        m_geomMesh.triangulate(currFace, newFaces, newHEs);

        if (newFaces.size() != 0 && newHEs.size() != 0) {
            // send name of the new face / half-edge
            for (Face* f : newFaces) {
                sendFace(f);
            }

            for (HalfEdge* he : newHEs) {
                sendHE(he);
            }

        }
        m_geomMesh.destroy();
        m_geomMesh.create();
        update();
    }
}

// triangulate current face
void MyGL::slot_subdivide(bool pressed) {
    if (pressed) {
        std::vector<Face*> addedFaces;
        std::vector<HalfEdge*> addedHEs;
        std::vector<Vertex *> addedVerts;
        m_geomMesh.subdivide(addedFaces, addedHEs, addedVerts);

        if (addedFaces.size() != 0 && addedHEs.size() != 0 && addedVerts.size() != 0) {
            // send name of the new face / half-edge
            for (Face* f : addedFaces) {
                sendFace(f);
            }

            for (HalfEdge* he : addedHEs) {
                sendHE(he);
            }

            for (Vertex* v : addedVerts) {
                sendVertex(v);
            }

        }
        m_geomMesh.destroy();
        m_geomMesh.create();
        update();
    }
}

// extrude current face
void MyGL::slot_extrude(bool pressed) {
    if (pressed && currFace != nullptr) {
        std::vector<Face*> addedFaces;
        std::vector<HalfEdge*> addedHEs;
        std::vector<Vertex *> addedVerts;
        m_geomMesh.extrude(currFace, addedVerts, addedFaces, addedHEs);

        if (addedFaces.size() != 0 && addedHEs.size() != 0 && addedVerts.size() != 0) {
            // send name of the new face / half-edge
            for (Face* f : addedFaces) {
                sendFace(f);
            }

            for (HalfEdge* he : addedHEs) {
                sendHE(he);
            }

            for (Vertex* v : addedVerts) {
                sendVertex(v);
            }
        }
        m_geomMesh.destroy();
        m_geomMesh.create();
        update();
    }
}

// load obj file
void MyGL::slot_loadOBJ(bool pressed) {
    if (pressed) {
        std::vector<Vertex*> vertices;
        std::vector<HalfEdge*> halfedges;
        std::vector<Face*> faces;
        QMap<std::pair<Vertex*, Vertex*>, HalfEdge*> hePairs;

        QString filename = QFileDialog::getOpenFileName(0,
                                                        QString("LoadOBJ"),
                                                        QString("../../"),
                                                        tr("*.obj"));
        QFile file(filename);

        if (file.open(QIODevice::ReadOnly | QIODevice::Text)) {
            // clear cube
            m_geomMesh.destroy();
            HalfEdge::resetID();
            Vertex::resetID();
            Face::resetID();

            QTextStream in(&file);
            QString line = in.readLine();

            while (!line.isNull()) {

                // parse through vertices
                if(line.indexOf(QString("v ")) == 0) {
                    QTextStream stream(&line);
                    // read in vertex position
                    // get rid of "v "
                    QString prefix;
                    float xPos;
                    float yPos;
                    float zPos;
                    stream >> prefix >> xPos >> yPos >> zPos;

                    glm::vec4 pos = glm::vec4(xPos, yPos, zPos, 1);

                    // Make new vertex
                    Vertex* v = new Vertex(pos);

                    // add vertex to list of vertices
                    vertices.push_back(v);
                }

                // parse through faces
                if(line.indexOf(QString("f ")) == 0) {
                    QTextStream stream(&line);

                    // get rid of "f "
                    QString prefix;
                    stream >> prefix;

                    std::vector<QString> strVec;
                    while (!stream.atEnd()) {
                        QString curr;
                        stream >> curr;
                        strVec.push_back(curr);
                    }

                    std::vector<QString> indices;
                    for (QString str : strVec) {
                        QString val = "";
                        for (int i = 0; i < str.length(); ++i) {
                            if (str.at(i) != "/") {
                                val += str.at(i);
                            } else {
                                break;
                            }
                        }
                        indices.push_back(val);
                    }

                    float r = ((float) rand() / (RAND_MAX));
                    float g = ((float) rand() / (RAND_MAX));
                    float b = ((float) rand() / (RAND_MAX));
                    Face* f = new Face(glm::vec4(r, g, b, 1));

                    /// we are creating our HEs as we're creating the face
                    /// for each half edge we create, we store it in a map
                    /// under a vertex pair (v1, v2) or (v2, v3)
                    /// if either (v1, v2) or (v2, v3) are in the map:
                    ///     -- get the half edge the pair points to and set
                    ///        that as the sym of the current HE
                    /// else:
                    ///     -- add the current vertex pair and map it to the current HE

                    // create half edges
                    std::vector<HalfEdge*> faceHEs;
                    for (int i = 0; i < indices.size(); ++i) {
                        int index = indices.at(i).toInt() - 1;
                        HalfEdge* he = new HalfEdge(f, vertices.at(index));

                        Vertex* currVert = vertices.at(index);
                        Vertex* prevVert;
                        if (i == 0) {
                            // last vertex in face list
                            prevVert =  vertices.at(indices.at(indices.size() - 1).toInt() - 1);
                        } else {
                            // previous vertex in face list
                            prevVert = vertices.at(indices.at(i - 1).toInt() - 1);
                        }
                        // set vertex pointer
                        currVert->setEdge(he);

                        // add to list of face HEs
                        faceHEs.push_back(he);

                        // add vertex pair and HE to map OR set sym pointer
                        std::pair<Vertex*, Vertex*> pair1 = std::make_pair(currVert, prevVert);
                        std::pair<Vertex*, Vertex*> pair2 = std::make_pair(prevVert, currVert);
                        if (hePairs.contains(pair1) || hePairs.contains(pair2)) {
                            std::pair<Vertex*, Vertex*> currPair;
                            if (hePairs.contains(pair1)) {
                                currPair = pair1;
                            } else {
                                currPair = pair2;
                            }

                            HalfEdge* sym = hePairs[currPair];
                            he->setSymHE(sym);
                        } else {
                            hePairs.insert(pair1, he);
                        }
                    }

                    // set face HE pointer
                    f->setHE(faceHEs.at(0));

                    // set next pointers
                    for (int i = 0; i < faceHEs.size(); i++) {
                        faceHEs.at(i)->setNextHE(faceHEs.at((i + 1) % faceHEs.size()));
                        halfedges.push_back(faceHEs.at(i));
                    }

                    faces.push_back(f);
                }

                line = in.readLine();
            }

            m_geomMesh.setFaces(faces);
            m_geomMesh.setHEs(halfedges);
            m_geomMesh.setVerts(vertices);
        }
        m_geomMesh.create();
        for (Vertex* v : m_geomMesh.getVerts()) {
            emit sendVertex(v);
        }

        for (Face* f : m_geomMesh.getFaces()) {
            emit sendFace(f);
        }

        for (HalfEdge* he : m_geomMesh.getHEs()) {
            emit sendHE(he);
        }
        update();
    }
}

// helper function which recursively sets children
void MyGL::setChildren(QJsonArray children, Joint* parent) {
    if (children.size() != 0) {
        for (int i = 0; i < children.size(); ++i) {
            QJsonObject curr = children.at(i).toObject();

            QString name = curr["name"].toString();
            QJsonArray currPos = curr["pos"].toArray();
            QJsonArray currRot = curr["rot"].toArray();
            QJsonArray currChildren = curr["children"].toArray();

            // get root position based on world space
            glm::vec4 pos = glm::vec4((float) currPos.at(0).toDouble(),
                                      (float) currPos.at(1).toDouble(),
                                      (float) currPos.at(2).toDouble(),
                                      1);

            // converts identity matrix to a rotation matrix using the rot values
            glm::mat4 rot = glm::rotate(glm::mat4(),
                                        glm::radians((float) currRot.at(0).toDouble()),
                                        glm::vec3((float) currRot.at(1).toDouble(),
                                                  (float) currRot.at(2).toDouble(),
                                                  (float) currRot.at(3).toDouble()));

            // convert to quatrenion
            glm::quat quat = glm::toQuat(rot);

            Joint* currJt = new Joint(name, parent, pos, quat, this);
            skeleton.push_back(currJt);

            setChildren(currChildren, currJt);
        }
    }
}

// load skeleton json file
void MyGL::slot_loadSkeleton(bool pressed) {
    if (pressed) {
        QString filename = QFileDialog::getOpenFileName(0,
                                                        QString("Load Skeleton"),
                                                        QString("../../"),
                                                        tr("*.json"));
        QFile file(filename);

        if (file.open(QIODevice::ReadOnly | QIODevice::Text)) {
            // destroy currently present skeleton
            for (Joint* jt : skeleton) {
                jt->destroy();
            }
            skeleton.clear();

            QString documentStr = file.readAll();
            file.close();

            QJsonDocument doc = QJsonDocument::fromJson(documentStr.toUtf8());
            QJsonObject docObj = doc.object();
            QJsonValue rootVal = docObj.value(QString("root"));
            QJsonObject root = rootVal.toObject();

            // parse first node
            QString rootName = root["name"].toString();
            QJsonArray rootPos = root["pos"].toArray();
            QJsonArray rootRot = root["rot"].toArray();
            QJsonArray children = root["children"].toArray();

            // get root position based on world space
            glm::vec4 pos = glm::vec4((float) rootPos.at(0).toDouble(),
                                      (float) rootPos.at(1).toDouble(),
                                      (float) rootPos.at(2).toDouble(),
                                      1);

            // make quaternion
            glm::quat quat = glm::angleAxis((float) rootRot.at(0).toDouble(),
                                            glm::vec3((float) rootRot.at(1).toDouble(),
                                                      (float) rootRot.at(2).toDouble(),
                                                      (float) rootRot.at(3).toDouble()));

            Joint* rootJt = new Joint(rootName, nullptr, pos, quat, this);

            skeleton.push_back(rootJt);

            // helper function:
            // goes through children and adds created joints to the vector of Joints
            setChildren(children, rootJt);

        }

        sendRoot(skeleton.at(0));

        // set children
        for (Joint* jt : skeleton) {
            Joint* parent = jt->getParent();
            if (parent != nullptr) {
                parent->addChild(jt);

            }
            jt->create();
        }

        update();
    }
}

// store currently selected joint
void MyGL::slot_storeCurrJoint(Joint* joint) {
    // deselect previously selected joint
    if (currJoint != nullptr) {
        currJoint->setSelected(false);
    }

    // set selected joint to be selected for drawing
    currJoint = joint;
    currJoint->setSelected(true);

    // send current position to gui
    sendJointPos(currJoint->getWorldPosition());
    for (Joint* jt : skeleton) {
        jt->destroy();
        jt->create();
    }

    update();
}

// set influence of the joints on each vertex in the mesh
void MyGL::slot_setVertexInfluence(bool pressed) {
    if (pressed) {
        for (Vertex* v : m_geomMesh.getVerts()) {
            Joint* jt1;
            float minDist1 = HUGE_VALF;

            // compute first closest joint
            for (Joint* jt : skeleton) {
                glm::vec4 vPos = v->getCoord();
                glm::vec4 jtPos = jt->getWorldPosition();
                float currDist = glm::distance(vPos, jtPos);
                if (currDist < minDist1) {
                    jt1 = jt;
                    minDist1 = currDist;
                }
            }

            Joint* jt2;
            float minDist2 = HUGE_VALF;
            // compute second closest joint
            for (Joint* jt : skeleton) {
                if (jt != jt1) {
                    glm::vec4 vPos = v->getCoord();
                    glm::vec4 jtPos = jt->getWorldPosition();
                    float currDist = glm::distance(vPos, jtPos);
                    if (currDist < minDist2) {
                        jt2 = jt;
                        minDist2 = currDist;
                    }
                }
            }

            // set vertex influence
            v->setInfluence(jt1, minDist1, jt2, minDist2);
        }
        skinPressed = true;
        setBindArray();
        setJointTrans();
        m_geomMesh.destroy();
        m_geomMesh.create();
        update();
    }
}

// change joint position with spin boxes

void MyGL::slot_changeJtX(double x) {
    if (currJoint != nullptr) {
        glm::vec4 coordW = currJoint->getWorldPosition();
        float diff = x - coordW[0];
        glm::vec4 coordL = currJoint->getLocalPosition();
        currJoint->setPosition(glm::vec4(coordL[0] + diff, coordL[1], coordL[2], 1));
        for (Joint* jt : skeleton) {
            jt->destroy();
            jt->create();
        }
        setJointTrans();
        update();
    }
}

void MyGL::slot_changeJtY(double y) {
    if (currJoint != nullptr) {
        glm::vec4 coordW = currJoint->getWorldPosition();
        float diff = y - coordW[1];
        glm::vec4 coordL = currJoint->getLocalPosition();
        currJoint->setPosition(glm::vec4(coordL[0], coordL[1] + diff, coordL[2], 1));
        for (Joint* jt : skeleton) {
            jt->destroy();
            jt->create();
        }
        setJointTrans();
        update();
    }
}

void MyGL::slot_changeJtZ(double z) {
    if (currJoint != nullptr) {
        glm::vec4 coordW = currJoint->getWorldPosition();
        float diff = z - coordW[2];
        glm::vec4 coordL = currJoint->getLocalPosition();
        currJoint->setPosition(glm::vec4(coordL[0], coordL[1], coordL[2] + diff, 1));
        for (Joint* jt : skeleton) {
            jt->destroy();
            jt->create();
        }
        setJointTrans();
        update();
    }
}

void MyGL::slot_changeJtXRot(bool pressed) {
    if (currJoint != nullptr && pressed) {
        glm::quat quat = currJoint->getQuaternion();
        quat = glm::angleAxis(glm::radians(5.f), glm::vec3(1,0,0)) * quat;
        currJoint->setQuat(quat);
        for (Joint* jt : skeleton) {
            jt->destroy();
            jt->create();
        }
        setJointTrans();
        update();
    }
}

void MyGL::slot_changeJtYRot(bool pressed) {
    if (currJoint != nullptr && pressed) {
        glm::quat quat = currJoint->getQuaternion();
        quat = glm::angleAxis(glm::radians(5.f), glm::vec3(0,1,0)) * quat;
        currJoint->setQuat(quat);
        for (Joint* jt : skeleton) {
            jt->destroy();
            jt->create();
        }
        setJointTrans();
        update();
    }
}

void MyGL::slot_changeJtZRot(bool pressed) {
    if (currJoint != nullptr && pressed) {
        glm::quat quat = currJoint->getQuaternion();
        quat = glm::angleAxis(glm::radians(5.f), glm::vec3(0,0,1)) * quat;
        currJoint->setQuat(quat);
        for (Joint* jt : skeleton) {
            jt->destroy();
            jt->create();
        }
        setJointTrans();
        update();
    }
}

// function to set bind matrix array
void MyGL::setBindArray() {
    prog_skeleton.useMe();

    glm::mat4 bindMatrices[100];

    // set bind array values
    for (int i = 0; i < skeleton.size(); i++) {
        Joint* jt = skeleton.at(i);
        jt->setBindMatrix();
        bindMatrices[i] = jt->getBindMatrix();
    }

    if (prog_skeleton.unifBindMatrices != -1) {
        this->glUniformMatrix4fv(prog_skeleton.unifBindMatrices,
                                 skeleton.size(),
                                 GL_FALSE,
                                 &(bindMatrices[0][0][0]));
    }

}

// function to set joint transformation array
void MyGL::setJointTrans() {
    prog_skeleton.useMe();
    glm::mat4 jointTrans[100];

    // set bind array values
    for (int i = 0; i < skeleton.size(); i++) {
        Joint* jt = skeleton.at(i);
        jointTrans[i] = jt->getOverallTransformation();
    }

    if (prog_skeleton.unifJointTrans != -1) {
        this->glUniformMatrix4fv(prog_skeleton.unifJointTrans,
                                 skeleton.size(),
                                 GL_FALSE,
                                 &(jointTrans[0][0][0]));
    }

}

