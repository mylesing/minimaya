#ifndef DRAWVERTEX_H
#define DRAWVERTEX_H

#include "drawable.h"
#include "vertex.h"

class drawVertex : public Drawable {
private:
    Vertex* vert;

public:
    // constructor
    drawVertex(GLWidget277* mp_context);

    ~drawVertex();

    // set vertex
    void setVert(Vertex* v);

    // change draw mode
    virtual GLenum drawMode() override;

    // create vertex selection
    virtual void create() override;
};

class drawHE : public Drawable {
private:
    HalfEdge* he;

public:
    // constructor
    drawHE(GLWidget277* mp_context);

    // set halfedge
    void setHE(HalfEdge* h);

    // change draw mode
    virtual GLenum drawMode() override;

    // create halfedge selection
    virtual void create() override;
};

class drawFace : public Drawable {
private:
    Face* face;

public:
    // constructor
    drawFace(GLWidget277* mp_context);

    // set vertex
    void setFace(Face* f);

    // change draw mode
    virtual GLenum drawMode() override;

    // create vertex selection
    virtual void create() override;
};

#endif // DRAWVERTEX_H
