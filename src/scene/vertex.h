#ifndef VERTEX_H
#define VERTEX_H
#include "joint.h"
#include <la.h>
#include <QListWidgetItem>

class HalfEdge;

/// VERTEX CLASS:

class Vertex : public QListWidgetItem {
private:
    glm::vec4 points;
    HalfEdge* vertHE;

    // influence joints have on the vertex
    std::vector<Joint*> joints;
    std::vector<float> influences;

    int id;

public:
    // id counter
    static int vertCount;

    // constructor
    Vertex(const glm::vec4 v);

    ~Vertex();

    // setter for half edge
    void setEdge(HalfEdge* he);

    // setter for coord
    void setCoord(const glm::vec4 coord);

    // getter for half edge
    HalfEdge* getEdge() const;

    // getter for coord
    glm::vec4 getCoord() const;

    // set influence given two joints and two distances
    void setInfluence(Joint* jt1, const float d1, Joint* jt2, const float d2);

    // get joints with inf on the current vertex
    std::vector<Joint*>  getJoints() const;

    // get influence on the current vertex
    std::vector<float>  getInfluence() const;

    // getter for id
    int getID() const;

    // reset ID
    static void resetID();
};

/// FACE CLASS:

class Face : public QListWidgetItem {
private:
    //std::vector<Vertex*> verts;
    HalfEdge* faceHE;
    glm::vec4 color;
    int id;

public:
    // id counter
    static int faceCount;

    // constructor
    Face(const glm::vec4 col);

    ~Face();

    // getter for half edge
    HalfEdge* getHE() const;

    // getter for color
    glm::vec4 getColor() const;

    // setter for half edge
    void setHE(HalfEdge* he);

    // setter for color
    void setColor(glm::vec4 col);

    // getter for id
    int getID() const;

    // reset ID
    static void resetID();
};

/// HALF-EDGE CLASS:

class HalfEdge : public QListWidgetItem {
private:
    HalfEdge* next;
    HalfEdge* sym;
    Face* face;
    Vertex* vert;
    int id;

public:
    // id counter
    static int heCount;

    // constructor
    HalfEdge(Face* f, Vertex* v);

    ~HalfEdge();

    // getter for next half edge
    HalfEdge* getNextHE() const;

    // getter for symmetrical half edge
    HalfEdge* getSymHE() const;

    // getter for face
    Face* getFace() const;

    // getter for vertex
    Vertex* getVert() const;

    // setter for next half edge
    void setNextHE(HalfEdge* he);

    // setter for symmetrical half edge
    void setSymHE(HalfEdge* he);

    // setter for face
    void setFace(Face* f);

    // setter for vertex
    void setVert(Vertex* v);

    // getter for id
    int getID() const;

    // reset ID
    static void resetID();
};

#endif // VERTEX_H
