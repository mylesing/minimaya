#include "vertex.h"

/// COUNT INITIALIZERS:

int Vertex::vertCount = 0;

int Face::faceCount = 0;

int HalfEdge::heCount = 0;

/// VERTEX FUNCTIONS:

// constructor
Vertex::Vertex(const glm::vec4 v) :
    points(v),
    id(vertCount++)
{
    setText(QString::number(id));
}

Vertex::~Vertex() {
}

// getter for half edge
void Vertex::setEdge(HalfEdge* he) {
    vertHE = he;
}

// getter for half edge
HalfEdge* Vertex::getEdge() const {
    return vertHE;
}

// setter for coord
void Vertex::setCoord(const glm::vec4 coord) {
    points = coord;
}

// getter for coord
glm::vec4 Vertex::getCoord() const {
    return points;
}

// set influence given two joints and two distances
void Vertex::setInfluence(Joint* jt1, const float d1, Joint* jt2, const float d2) {
    // compute influence as a percentage
    float total = d1 + d2;
    float infJt1 = d1 / total;
    float infJt2 = d2 / total;

    joints.push_back(jt1);
    joints.push_back(jt2);

    influences.push_back(infJt1);
    influences.push_back(infJt2);
}

// get joints with inf on the current vertex
std::vector<Joint*> Vertex::getJoints() const {
    return joints;
}

// get influence on the current vertex
std::vector<float> Vertex::getInfluence() const {
    return influences;
}

// getter for id
int Vertex::getID() const {
    return id;
}

// reset ID
void Vertex::resetID() {
    vertCount = 0;
}


/// FACE FUNCTIONS:

// constuctor
Face::Face(const glm::vec4 col) :
    color(col),
    id(faceCount++)
{
    setText(QString::number(id));
}

Face::~Face() {
}

// getter for half edge
HalfEdge* Face::getHE() const {
    return faceHE;
}

// getter for color
glm::vec4 Face::getColor() const {
    return color;
}

// setter for half edge
void Face::setHE(HalfEdge* he) {
    faceHE = he;
}

// setter for color
void Face::setColor(glm::vec4 col) {
    color = col;
}

// getter for id
int Face::getID() const {
    return id;
}

// reset ID
void Face::resetID() {
    faceCount = 0;
}

/// HALF-EDGE FUNCTIONS:

// constructor
HalfEdge::HalfEdge(Face* f, Vertex* v) :
    face(f),
    vert(v),
    id(heCount++)
{
    setText(QString::number(id));
}

HalfEdge::~HalfEdge() {
}

// getter for next half edge
HalfEdge* HalfEdge::getNextHE() const {
    return next;
}

// getter for symmetrical half edge
HalfEdge* HalfEdge::getSymHE() const {
    return sym;
}

// getter for face
Face* HalfEdge::getFace() const {
    return face;
}

// getter for vertex
Vertex* HalfEdge::getVert() const {
    return vert;
}

// setter for next half edge
void HalfEdge::setNextHE(HalfEdge* he) {
    next = he;
}

// setter for symmetrical half edge
void HalfEdge::setSymHE(HalfEdge* he) {
    sym = he;
    he->sym = this;
}

// setter for face
void HalfEdge::setFace(Face* f) {
    face = f;
}

// setter for vertex
void HalfEdge::setVert(Vertex* v) {
    vert = v;
}

// getter for id
int HalfEdge::getID() const {
    return id;
}

// reset ID
void HalfEdge::resetID() {
    heCount = 0;
}
