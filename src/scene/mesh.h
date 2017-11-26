#ifndef MESH_H
#define MESH_H

#include "drawable.h"
#include "vertex.h"
#include <la.h>

class Mesh : public Drawable {

private:
    std::vector<Vertex*> vertices;
    std::vector<Face*> faces;
    std::vector<HalfEdge*> halfedges;

public:
    // constructor
    Mesh(GLWidget277* mp_context);

    ~Mesh();

    // set vertices
    void setVerts(const std::vector<Vertex*> verts);

    // set faces
    void setFaces(const std::vector<Face*> fs);

    // set half edges
    void setHEs(const std::vector<HalfEdge*> hes);

    // get half edges
    std::vector<HalfEdge*> getHEs() const;

    // get vertices
    std::vector<Vertex*> getVerts() const;

    // get faces
    std::vector<Face*> getFaces() const;

    // split edge
    void splitEdge(HalfEdge* he);

    // triangulate
    void triangulate(Face* face,
                     std::vector<Face*> &newFaces,
                     std::vector<HalfEdge*> &newHEs);
    // subdivide
    void subdivide(std::vector<Face *> &addedFaces,
                   std::vector<HalfEdge *> &addedHEs,
                   std::vector<Vertex *> &addedVerts);

    // extrude
    void extrude(Face* face, std::vector<Vertex *> &addedVerts,
                 std::vector<Face *> &addedFaces,
                 std::vector<HalfEdge *> &addedHEs);

    // create function
    virtual void create() override;
};

#endif // MESH_H
