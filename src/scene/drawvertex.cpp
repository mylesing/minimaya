#include "drawvertex.h"

drawVertex::drawVertex(GLWidget277* context) : Drawable(context)
{}

drawVertex::~drawVertex() {
}

// set vertex
void drawVertex::setVert(Vertex* v) {
    vert = v;
}

GLenum drawVertex::drawMode() {
    return GL_POINTS;
}

// create vertex selection
void drawVertex::create() {
    std::vector<GLuint> vidx = {0};
    std::vector<glm::vec4> vpos = {vert->getCoord()};
    std::vector<glm::vec4> vcol = {glm::vec4(1, 1, 1, 1)};

    count = 1;

    // Create a VBO on our GPU and store its handle in bufIdx
    generateIdx();
    mp_context->glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, bufIdx);
    mp_context->glBufferData(GL_ELEMENT_ARRAY_BUFFER,
                             vidx.size() * sizeof(GLuint),
                             vidx.data(), GL_STATIC_DRAW);

    generatePos();
    mp_context->glBindBuffer(GL_ARRAY_BUFFER, bufPos);
    mp_context->glBufferData(GL_ARRAY_BUFFER,
                             vpos.size() * sizeof(glm::vec4),
                             vpos.data(), GL_STATIC_DRAW);

    generateCol();
    mp_context->glBindBuffer(GL_ARRAY_BUFFER, bufCol);
    mp_context->glBufferData(GL_ARRAY_BUFFER,
                             vcol.size() * sizeof(glm::vec4),
                             vcol.data(), GL_STATIC_DRAW);

}

drawHE::drawHE(GLWidget277* context) : Drawable(context)
{}

// set halfedge
void drawHE::setHE(HalfEdge* h) {
    he = h;
}

// change draw mode
GLenum drawHE::drawMode() {
    return GL_LINES;
}

// create halfedge selection
void drawHE::create() {
    std::vector<GLuint> idx = {0, 1};
    std::vector<glm::vec4> pos = {he->getSymHE()->getVert()->getCoord(),
                                  he->getVert()->getCoord()};
    std::vector<glm::vec4> col = {glm::vec4(1, 0, 0, 1), glm::vec4(1, 1, 0, 1)};

    count = 2;

    // Create a VBO on our GPU and store its handle in bufIdx
    generateIdx();
    mp_context->glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, bufIdx);
    mp_context->glBufferData(GL_ELEMENT_ARRAY_BUFFER,
                             idx.size() * sizeof(GLuint),
                             idx.data(), GL_STATIC_DRAW);

    generatePos();
    mp_context->glBindBuffer(GL_ARRAY_BUFFER, bufPos);
    mp_context->glBufferData(GL_ARRAY_BUFFER,
                             pos.size() * sizeof(glm::vec4),
                             pos.data(), GL_STATIC_DRAW);

    generateCol();
    mp_context->glBindBuffer(GL_ARRAY_BUFFER, bufCol);
    mp_context->glBufferData(GL_ARRAY_BUFFER,
                             col.size() * sizeof(glm::vec4),
                             col.data(), GL_STATIC_DRAW);
}

// constructor
drawFace::drawFace(GLWidget277* context) : Drawable(context)
{}

// set face
void drawFace::setFace(Face* f) {
    face = f;
}

// change draw mode
GLenum drawFace::drawMode() {
    return GL_LINES;
}

// create face selection
void drawFace::create() {
    std::vector<GLuint> idx;
    std::vector<glm::vec4> pos;
    std::vector<glm::vec4> col;

    // get start edge
    HalfEdge* start = face->getHE();

    // initialize first values
    pos.push_back(start->getVert()->getCoord());
    col.push_back(glm::vec4(1, 1, 1, 2) - face->getColor());

    // continue along face
    int idCount = 1;
    for (HalfEdge* e = start->getNextHE(); e != start; e = e->getNextHE()) {
        pos.push_back(e->getVert()->getCoord());
        col.push_back(glm::vec4(1, 1, 1, 2) - face->getColor());
        idCount++;
    }

    for (int i = 0; i < idCount; ++i) {
        if (i % idCount == idCount - 1) {
            idx.push_back(i);
            idx.push_back(0);
        } else {
            idx.push_back(i);
            idx.push_back(i + 1);
        }
    }

    count = idx.size();

    // Create a VBO on our GPU and store its handle in bufIdx
    generateIdx();
    mp_context->glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, bufIdx);
    mp_context->glBufferData(GL_ELEMENT_ARRAY_BUFFER,
                             idx.size() * sizeof(GLuint),
                             idx.data(), GL_STATIC_DRAW);

    generatePos();
    mp_context->glBindBuffer(GL_ARRAY_BUFFER, bufPos);
    mp_context->glBufferData(GL_ARRAY_BUFFER,
                             pos.size() * sizeof(glm::vec4),
                             pos.data(), GL_STATIC_DRAW);

    generateCol();
    mp_context->glBindBuffer(GL_ARRAY_BUFFER, bufCol);
    mp_context->glBufferData(GL_ARRAY_BUFFER,
                             col.size() * sizeof(glm::vec4),
                             col.data(), GL_STATIC_DRAW);
}
