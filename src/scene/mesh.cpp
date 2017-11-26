#include "mesh.h"
#include <iostream>
#include <set>
#include <map>

// constructor
Mesh::Mesh(GLWidget277 *context) : Drawable(context)
{}

Mesh::~Mesh() {
    for (Vertex* v : vertices) {
        delete v;
    }

    for (HalfEdge* he : halfedges) {
        delete he;
    }

    for (Face* f : faces) {
        delete f;
    }
}

// set vertices
void Mesh::setVerts(const std::vector<Vertex*> verts) {
    vertices = verts;
}

// set faces
void Mesh::setFaces(const std::vector<Face*> fs) {
    faces = fs;
}

// set half edges
void Mesh::setHEs(const std::vector<HalfEdge*> hes) {
    halfedges = hes;
}

// get vertices
std::vector<Vertex*> Mesh::getVerts() const {
    return vertices;
}

// get faces
std::vector<Face*> Mesh::getFaces() const {
    return faces;
}

// get half edges
std::vector<HalfEdge*> Mesh::getHEs() const {
    return halfedges;
}

// split edge
void Mesh::splitEdge(HalfEdge* he) {
    glm::vec4 start = he->getSymHE()->getVert()->getCoord();
    glm::vec4 end = he->getVert()->getCoord();

    glm::vec4 newPos = glm::vec4((start[0] + end[0]) / 2,
            (start[1] + end[1]) / 2,
            (start[2] + end[2]) / 2,
            1);
    Vertex* newVert = new Vertex(newPos);
    HalfEdge* newHE = new HalfEdge(he->getFace(), he->getVert());

    // get the current half edge's necxt value
    HalfEdge* next = he->getNextHE();

    // change stored variables of the old half edge
    he->setVert(newVert);
    he->setNextHE(newHE);

    // set stored variables for new half edge
    newHE->setNextHE(next);
    newHE->setSymHE(he->getSymHE());

    // add them to the vectors
    vertices.push_back(newVert);
    halfedges.push_back(newHE);
}

void Mesh::triangulate(Face *face,
                       std::vector<Face*> &newFaces,
                       std::vector<HalfEdge*> &newHEs) {

    /// triangulating an n-gon:
    /// 0: loop around face and store the half edges and vertices of the face in vectors!
    /// 1: create (n - 3) new faces (n - 2 in total, 1 face is current face)
    ///         -- store the new Faces in a separate vector
    /// 2: create new half-edges going from 0 to 1 - (n-1) vertices
    ///         -- 2 * (n - 3) half edges
    ///         -- for each pair of new HEs, set them to be each other's sym
    ///         -- store the new HEs in a separate vector
    /// 3: fix next pointers:
    ///         -- if i == 0: newHE(0) > oldHE(1) > oldHE(2) > newHE(0)
    ///         -- if i == last: newHE(last) > oldHE(last) > oldHE(0) > newHE(last)
    ///         -- else: newHE(i) > oldHE(j) > newHE(i + 1) > newHE(i) (3 =< j =< size - 2)

    // STEP 0: STORE ORIGINAL MESH COMPONENS

    // get start edge
    HalfEdge* start = face->getHE();

    // get the vertices on the face
    std::vector<Vertex*> faceVert;

    // get the original HE's of face
    std::vector<HalfEdge*> oldHEs;

    // push back first position
    faceVert.push_back(start->getVert());
    oldHEs.push_back(start);

    // loop around face and add the vertices and HEs
    for (HalfEdge* e = start->getNextHE(); e != start; e = e->getNextHE()) {
        faceVert.push_back(e->getVert());
        oldHEs.push_back(e);
    }

    // shouldn't try it on a triangle!!
    if (oldHEs.size() > 3) {

        // STEP 1: CREATE NEW FACES

        // add original face, for looping purposes
        newFaces.push_back(face);

        // create new faces
        for (int i = 0; i <  oldHEs.size() - 3; ++i) {
            Face* f = new Face(glm::vec4(1) - face->getColor());
            newFaces.push_back(f);
        }

        // STEP 2: CREATE NEW HEs

        // create new HEs
        for (int i = 0; i < oldHEs.size() - 3; ++i) {
            HalfEdge* he1 = new HalfEdge(newFaces.at(i), faceVert.at(0));
            HalfEdge* he2 = new HalfEdge(newFaces.at(i + 1), faceVert.at(i + 2));

            // set them to point at each other
            he1->setSymHE(he2);
            he2->setSymHE(he1);

            newHEs.push_back(he1);
            newHEs.push_back(he2);
        }

        HalfEdge* first = newHEs.at(0);
        HalfEdge* last = newHEs.at(newHEs.size() - 1);

        // set first triangle
        newFaces.at(0)->setHE(first);
        first->setNextHE(oldHEs.at(1));
        oldHEs.at(2)->setNextHE(first);

        // set last triangle
        newFaces.at(newFaces.size() - 1)->setHE(last);
        last->setNextHE(oldHEs.at(oldHEs.size() - 1));
        oldHEs.at(0)->setNextHE(last);

        // set pointers for the rest of the triangles
        int count = 1;
        for (int i  =  3; i < oldHEs.size() - 2; ++i) {
            // set face pointer
            newFaces.at(i - 1)->setHE(newHEs.at(count));

            // set HE pointers
            newHEs.at(count)->setNextHE(oldHEs.at(i));
            oldHEs.at(i)->setNextHE(newHEs.at(count + 1));

            // increment count
            count += 2;
        }

        // STEP 4: ADD NEW FACES AND HES TO THE LISTS
        for (Face* f : newFaces) {
            if (f != newFaces.at(0)) {
                faces.push_back(f);
            }
        }

        for (HalfEdge* he : newHEs) {
            halfedges.push_back(he);
        }
    }
}

void quadrangulate(Face* face,
                   Vertex* centroid,
                   QSet<Vertex*> &midpoints,
                   std::vector<HalfEdge*> &halfedges,
                   std::vector<Face*> &faces) {
    /// quadrangulate an n-gon:
    /// 1. Create n - 1 new faces
    ///     -- n = number of midpoints
    ///     -- the first face will be the original face
    /// 2. Create n new edges pointing from each midpoint to the centroid
    ///     -- 2(n) in total
    ///     -- Each new HE will have a face that it points points to
    ///     -- loop through face indeces
    ///     -- set each pair of newly created HE's to be each other's sym pointes
    /// 3. fix next pointers:
    ///     -- if i = 0: oldHE(0) > newHE(0) > oldHE(last) > newHE(last) > oldHE(0)
    ///     -- else: oldHE(i) > newHE(i) > oldHE(i + 1) > newHE(i + 1) > oldHE(i)

    HalfEdge* start = face->getHE();
    std::vector<HalfEdge*> oldHEs = {start};
    std::vector<Vertex*> orderedMids;
    if (midpoints.contains(start->getVert())) {
        orderedMids.push_back(start->getVert());
    }

    // loop around face and add the HEs and verts
    for (HalfEdge* e = start->getNextHE(); e != start; e = e->getNextHE()) {
        oldHEs.push_back(e);
        Vertex* v = e->getVert();
        if (midpoints.contains(v)) {
            orderedMids.push_back(e->getVert());
        }
    }

    // STEP 1: CREATE NEW FACES

    // stores subdivided faces
    std::vector<Face*> newFaces;

    // add original face, for looping purposes
    newFaces.push_back(face);

    // create n - 1 new faces
    for (int i = 0; i <  midpoints.size() - 1; ++i) {
        Face* f = new Face(face->getColor());
        newFaces.push_back(f);
    }

    // STEP 2: CREATE NEW HEs

    // stores newly created HEs
    std::vector<HalfEdge*> newHEs;

    // create new HEs
    for (int i = 0; i < orderedMids.size(); ++i) {
        HalfEdge* he1 = new HalfEdge(newFaces.at(i), centroid);
        HalfEdge* he2 = new HalfEdge(newFaces.at((i + 1) % newFaces.size()), orderedMids.at(i));

        newFaces.at(i)->setHE(he1);
        newFaces.at((i + 1) % newFaces.size())->setHE(he2);

        // set them to point at each other
        he1->setSymHE(he2);
        he2->setSymHE(he1);

        newHEs.push_back(he1);
        newHEs.push_back(he2);
    }

    // STEP 3: SET UP NEXT POINTERS

    /// 3. fix next pointers:
    ///     -- if i = 0: oldHE(0) > newHE(0) > oldHE(last) > newHE(last) > oldHE(0)
    ///     -- else: oldHE(i) > oldHE(i + 1) > newHE(i + 1) > newHE(i) > oldHE(i)

    HalfEdge* newFirst = newHEs.at(0);
    HalfEdge* newLast = newHEs.at(newHEs.size() - 1);
    HalfEdge* oldFirst = oldHEs.at(0);
    HalfEdge* oldLast = oldHEs.at(oldHEs.size() - 1);

    // set first quad
    oldFirst->setNextHE(newFirst);
    newFirst->setNextHE(newLast);
    newLast->setNextHE(oldLast);
    oldLast->setNextHE(oldFirst);

    // set pointers for the rest of the quads
    int count = 1;
    for (int i  =  1; i < orderedMids.size(); ++i) {

        // set HE pointers:
        // oldHE(i + 1) > newHE(i + 1)
        // newHE(i + 1) > newHE(i)
        // newHE(i) > oldHE(i)
        oldHEs.at(count + 1)->setNextHE(newHEs.at(count + 1));
        newHEs.at(count + 1)->setNextHE(newHEs.at(count));
        newHEs.at(count)->setNextHE(oldHEs.at(count));

        // increment count
        count += 2;
    }

    // STEP 4: SET UP FACE POINTERS
    for (Face* f : newFaces) {
        HalfEdge* currSt = f->getHE();
        currSt->setFace(f);
        for (HalfEdge* e = currSt->getNextHE(); e != currSt; e = e->getNextHE()) {
            e->setFace(f);
        }
    }

    // STEP 5: ADD NEW FACES AND HES TO THE LISTS
    for (Face* f : newFaces) {
        if (f != newFaces.at(0)) {
            faces.push_back(f);
        }
    }

    for (HalfEdge* he : newHEs) {
        halfedges.push_back(he);
    }

}

void Mesh::subdivide(std::vector<Face*> &addedFaces,
                     std::vector<HalfEdge*> &addedHEs,
                     std::vector<Vertex *> &addedVerts) {

    // stores the faces incident to a given vertex
    std::map<Vertex*, std::set<Face*>> facesPerVert;

    // maps the centroids to each face
    std::map<Face*, Vertex*> faceToCentroid;

    // stores centroids:
    std::vector<Vertex*> centroids;

    // STEP 1: Create a map which, for each vertex,
    //         stores a set of all faces associated with it
    for (Face* f : faces) {
        // loop through the HEs and gather the vertices
        // then for each vertex add the current face in the map

        HalfEdge* start = f->getHE();

        // store vertices on face
        std::vector<Vertex*> faceVerts;
        faceVerts.push_back(start->getVert());

        // store HEs on face
        std::vector<HalfEdge*> faceHEs;
        faceHEs.push_back(start);

        // loop around face and add vertices
        for (HalfEdge* e = start->getNextHE(); e != start; e = e->getNextHE()) {
            faceVerts.push_back(e->getVert());
            faceHEs.push_back(e);
        }

        glm::vec4 cenPos = glm::vec4(0, 0, 0, 0);
        for (Vertex* v: faceVerts) {
            std::set<Face*> currFaces;
            // if the vertex is not in the map then add
            if (facesPerVert.find(v) == facesPerVert.end()) {
                currFaces = {f};
                facesPerVert.insert(std::pair<Vertex*, std::set<Face*>>(v, currFaces));
                // if the vertex is in the map then get set, add face
            } else {
                currFaces = facesPerVert[v];
                currFaces.insert(f);
                facesPerVert.at(v) = currFaces;
            }
            cenPos += v->getCoord();
        }

        // STEP 2: compute face centroid and map it to a vertex

        // average out position
        cenPos /= faceVerts.size();

        // create centroid
        Vertex* centroid = new Vertex(cenPos);

        // set centroid's HE
        centroid->setEdge(f->getHE());

        // map the face to its centroid
        faceToCentroid.insert(std::pair<Face*, Vertex*>(f, centroid));

        // add centroid to list of centroid
        centroids.push_back(centroid);
    }

    // STEP 3: create midpoints and reset those pointers

    // stores midpoints
    std::vector<Vertex*> midpoints = std::vector<Vertex*>();

    // stores newly created HEs
    std::vector<HalfEdge*> newHEs = std::vector<HalfEdge*>();
    
    // maps the HEs to the face they belong to
    std::map<Face*, QSet<Vertex*>> midPerFace = std::map<Face*, QSet<Vertex*>>();

    // maps the HEs to the face they belong to
    std::map<Vertex*, std::vector<Vertex*>> midPerVert = std::map<Vertex*, std::vector<Vertex*>>();

    // set of halfedges which we've already gone through
    QSet<HalfEdge*> completedHEs;

    // go through each half edge
    for (HalfEdge* he : halfedges) {
        // only do edges which we have not yet gone through!
        if(!completedHEs.contains(he) && !completedHEs.contains(he->getSymHE())) {

            completedHEs.insert(he);
            completedHEs.insert(he->getSymHE());

            // get v1, v2, f1, and f2
            // vertices at each end
            glm::vec4 v1 = he->getSymHE()->getVert()->getCoord();
            glm::vec4 v2 = he->getVert()->getCoord();

            // centroids
            glm::vec4 f1 = faceToCentroid[he->getFace()]->getCoord();
            glm::vec4 f2 = faceToCentroid[he->getSymHE()->getFace()]->getCoord();

            glm::vec4 midPos = v1 + v2 + f1 + f2;
            midPos /= 4;

            // create midpoint vertex from calculated position
            Vertex* midVert = new Vertex(midPos);
            midVert->setEdge(he);

            // map the midpoint to the vertices it's connected to
            for (int i = 0; i < 2; ++i) {
                Vertex* currVert;
                if (i == 0) {
                    currVert = he->getVert();
                } else {
                    currVert= he->getSymHE()->getVert();
                }

                std::vector<Vertex*> currMids = std::vector<Vertex*>();
                // if the vertex is not in the map then add new vertex/midpoint pair
                if (midPerVert.find(currVert) == midPerVert.end()) {
                    currMids = {midVert};
                    midPerVert.insert(std::pair<Vertex*, std::vector<Vertex*>>(currVert, currMids));
                    // if the vertex is in the map then get set, add midpoint to its set
                } else {
                    currMids = midPerVert[currVert];
                    currMids.push_back(midVert);
                    midPerVert.at(currVert) = currMids;
                }
            }

            // map current midpoint to the faces it belongs to
            for (int i = 0; i < 2; ++i) {
                Face* currFace;
                if (i == 0) {
                    currFace = he->getFace();
                } else {
                    currFace= he->getSymHE()->getFace();
                }

                QSet<Vertex*> currMids = QSet<Vertex*>();
                // if the face is not in the map then add new vertex/midpoint pair
                if (midPerFace.find(currFace) == midPerFace.end()) {
                    currMids.insert(midVert);
                    midPerFace.insert(std::pair<Face*, QSet<Vertex*>>(currFace, currMids));
                    // if the face is in the map then get set, add midpoint to its set
                } else {
                    currMids = midPerFace[currFace];
                    currMids.insert(midVert);
                    midPerFace.at(currFace) = currMids;
                }
            }

            // create new HEs pointing towards the original vertices
            HalfEdge* newHE1 = new HalfEdge(he->getFace(), he->getVert());
            HalfEdge* newHE2 = new HalfEdge(he->getSymHE()->getFace(), he->getSymHE()->getVert());

            // get the current half edge's next HE
            HalfEdge* next1 = he->getNextHE();
            HalfEdge* next2 = he->getSymHE()->getNextHE();

            // set old HEs to point to the midpoints
            he->setVert(midVert);
            he->getSymHE()->setVert(midVert);

            // set old HEs to have the new HEs as their next HEs
            he->setNextHE(newHE1);
            he->getSymHE()->setNextHE(newHE2);

            // set new HEs to have the next HEs of the old HEs
            newHE1->setNextHE(next1);
            newHE2->setNextHE(next2);

            // set sym pointers
            newHE1->setSymHE(he->getSymHE());
            newHE2->setSymHE(he);

            // add them to the vectors
            midpoints.push_back(midVert);
            newHEs.push_back(newHE1);
            newHEs.push_back(newHE2);
        }
    }

    // STEP 5: SMOOTH VERTICES
    for (Vertex* v : vertices) {
        if (v->getID() == 28) {
            int x = 0;
        }
        std::vector<Vertex*> mids = midPerVert[v];
        glm::vec4 sumE = glm::vec4(0);
        for (Vertex* v : mids) {
            sumE += v->getCoord();
        }
        glm::vec4 sumC = glm::vec4(0);
        for (Face* f : facesPerVert[v]) {
            sumC += faceToCentroid[f]->getCoord();
        }
        int n = mids.size();

        glm::vec4 vCoord = v->getCoord();
        vCoord *= (n / 2);
        vCoord /= n;
        sumE /= std::pow(n, 2);
        sumC /= std::pow(n, 2);
        glm::vec4 newCoord = vCoord + sumE + sumC;

        v->setCoord(newCoord);
    }

    // STEP 6: QUADRANGULATE

    std::vector<Face*> newFaces = std::vector<Face*>();

    for (Face* f: faces) {
        // have a separate quadrangualte function
        quadrangulate(f,
                      faceToCentroid[f],
                      midPerFace[f],
                      newHEs,
                      newFaces);
    }

    // STEP 7: ADD ALL NEW
    for (Vertex* c : centroids) {
        vertices.push_back(c);
        addedVerts.push_back(c);
    }

    for (Vertex* m: midpoints) {
        vertices.push_back(m);
        addedVerts.push_back(m);
    }

    for (HalfEdge* he : newHEs) {
        halfedges.push_back(he);
        addedHEs.push_back(he);
    }

    for (Face* face : newFaces) {
        faces.push_back(face);
        addedFaces.push_back(face);
    }
}

void Mesh::extrude(Face* face,
                   std::vector<Vertex*> &addedVerts,
                   std::vector<Face*> &addedFaces,
                   std::vector<HalfEdge*> &addedHEs) {

    // STEP 1: store all of the vertices and HEs on face
    HalfEdge* start = face->getHE();
    std::vector<HalfEdge*> faceHEs = {start};
    std::vector<Vertex*> faceVerts = {start->getVert()};

    // loop around face and add the HEs and verts
    for (HalfEdge* e = start->getNextHE(); e != start; e = e->getNextHE()) {
        faceHEs.push_back(e);
        faceVerts.push_back(e->getVert());
    }

    // STEP 2: calculate face normal
    glm::vec4 p1 = faceVerts.at(0)->getCoord();
    glm::vec4 p2 = faceVerts.at(1)->getCoord();
    glm::vec4 p3 = faceVerts.at(2)->getCoord();
    glm::vec4 side1 = p2 - p1;
    glm::vec4 side2 = p3 - p2;
    glm::vec3 cross = glm::normalize(glm::cross(glm::vec3(side1[0], side1[1], side1[2]),
            glm::vec3(side2[0], side2[1], side2[2])));
    glm::vec4 norm = glm::vec4(cross[0], cross[1], cross[2], 0);

    // STEP 3: create new vertices which have been extruded along normal
    std::vector<Vertex*> extrudedVerts = std::vector<Vertex*>();
    for (Vertex* v : faceVerts) {
        glm::vec4 coord = (v->getCoord()) + norm;
        Vertex* extVert = new Vertex(coord);
        extrudedVerts.push_back(extVert);
    }

    // STEP 4: set original HEs to point at the new vertices
    for (int i = 0; i < faceHEs.size(); ++i) {
        faceHEs.at(i)->setVert(extrudedVerts.at(i));
    }

    // STEP 5: create new half edges which
    // will point to the HEs and their original Syms

    std::vector<Face*> newFaces = std::vector<Face*>();
    std::vector<HalfEdge*> newHEs = std::vector<HalfEdge*>();
    for (int i = 0; i < faceHEs.size(); ++i) {
        Face* newFace = new Face(glm::vec4(1) - face->getColor());
        int id = i - 1;
        if (i == 0) {
            id = faceHEs.size() - 1;
        }
        HalfEdge* curr = faceHEs.at(i);
        HalfEdge* currSym = curr->getSymHE();

        HalfEdge* newHE1 = new HalfEdge(newFace, extrudedVerts.at(id));
        HalfEdge* newHE2 = new HalfEdge(newFace, faceVerts.at(i));

        // set vertex HE pointer
        extrudedVerts.at(id)->setEdge(newHE1);

        // set face HE pointer
        newFace->setHE(newHE1);

        // set sym pointers
        curr->setSymHE(newHE1);
        currSym->setSymHE(newHE2);

        // add to vectors
        newFaces.push_back(newFace);
        newHEs.push_back(newHE1);
        newHEs.push_back(newHE2);

    }

    // STEP 6: go through the faces and set half edges there
    std::vector<HalfEdge*> newSideHEs = std::vector<HalfEdge*>();
    int count = 0;
    for (int i = 0; i < faceHEs.size(); ++i) {
        int id = i - 1;
        if (i == 0) {
            id = faceHEs.size() - 1;
        }
        HalfEdge* newHE1 = new HalfEdge(newFaces.at(i), faceVerts.at(id));
        HalfEdge* newHE2 = new HalfEdge(newFaces.at(i), extrudedVerts.at(i));

        // set next pointers
        newHE1->setNextHE(newHEs.at(count + 1));
        newHEs.at(count + 1)->setNextHE(newHE2);
        newHE2->setNextHE(newHEs.at(count));
        newHEs.at(count)->setNextHE(newHE1);

        newSideHEs.push_back(newHE1);
        newSideHEs.push_back(newHE2);

        count += 2;
    }

    // STEP 7: set sym pointers

    // set last edge pair
    newSideHEs.at(0)->setSymHE(newSideHEs.at(newSideHEs.size() - 1));
    newSideHEs.at(newSideHEs.size() - 1)->setSymHE(newSideHEs.at(0));

    // set the rest
    for (int i = 1; i < newSideHEs.size() - 1; i += 2) {
        newSideHEs.at(i)->setSymHE(newSideHEs.at(i + 1));
        newSideHEs.at(i + 1)->setSymHE(newSideHEs.at(i));
    }

    // add all created mesh objects to their sets
    for (Vertex* v : extrudedVerts) {
        vertices.push_back(v);
        addedVerts.push_back(v);
    }

    for (HalfEdge* he : newHEs) {
        halfedges.push_back(he);
        addedHEs.push_back(he);
    }

    for (HalfEdge* he : newSideHEs) {
        halfedges.push_back(he);
        addedHEs.push_back(he);
    }

    for (Face* f : newFaces) {
        faces.push_back(f);
        addedFaces.push_back(f);
    }
}



// get mesh positions
void setup(std::vector<Face*> &faces,
           std::vector<GLuint> &idx,
           std::vector<glm::vec4> &positions,
           std::vector<glm::vec4> &normals,
           std::vector<glm::vec4> &colors,
           std::vector<glm::ivec2> &joints,
           std::vector<glm::vec2> &influences) {

    // set up startID for each face for triangulization
    int startID = 0;

    // go through the faces
    for (Face* f : faces) {
        // get start edge
        HalfEdge* start = f->getHE();

        // get the positions on the face, for normalization
        std::vector<glm::vec4> facePos;

        // get face vertices
        std::vector<Vertex*> faceVert;

        // push back first position
        positions.push_back(start->getVert()->getCoord());
        facePos.push_back(start->getVert()->getCoord());
        faceVert.push_back(start->getVert());

        // count number of vertices on face
        int verts = 1;

        // loop around face and add the vertex positions
        for (HalfEdge* e = start->getNextHE(); e != start; e = e->getNextHE()) {
            positions.push_back(e->getVert()->getCoord());
            facePos.push_back(e->getVert()->getCoord());
            faceVert.push_back(e->getVert());
            verts++;
        }

        for (Vertex* v : faceVert) {
            std::vector<Joint*> jts = v->getJoints();
            std::vector<float> inf = v->getInfluence();
            if (jts.size() != 0) {
                joints.push_back(glm::ivec2(jts.at(0)->getID(), jts.at(1)->getID()));
                influences.push_back(glm::vec2(inf.at(0), inf.at(1)));
            }
        }

        // set triangulization indices
        for (int i = 0; i < verts - 2; ++i) {
            idx.push_back(startID);
            idx.push_back(startID + i + 1);
            idx.push_back(startID + i + 2);
        }
        // increment startID to have number of vertices of last face indexed
        startID += verts;

        for (int i = 0; i < verts; ++i) {
            // set color for each vertex on the face
            colors.push_back(f->getColor());

            // calculate normal per vertex
            glm::vec4 p1;
            glm::vec4 p2;
            glm::vec4 p3;
            if (i == 0) {
                p1 = facePos.at(i + verts - 1);
                p2 = facePos.at(i);
                p3 = facePos.at(i + 1);
            } else if (i % verts == verts - 1) {
                p1 = facePos.at(i - 1);
                p2 = facePos.at(i);
                p3 = facePos.at(i - verts + 1);
            } else {
                p1 = facePos.at(i - 1);
                p2 = facePos.at(i);
                p3 = facePos.at(i + 1);
            }
            glm::vec4 side1 = p2 - p1;
            glm::vec4 side2 = p3 - p2;
            glm::vec3 cross = glm::normalize(glm::cross(glm::vec3(side1[0], side1[1], side1[2]),
                    glm::vec3(side2[0], side2[1], side2[2])));
            glm::vec4 norm = glm::vec4(cross[0], cross[1], cross[2], 0);
            normals.push_back(norm);
        }
    }
}

void Mesh::create() {
    std::vector<GLuint> mesh_idx;
    std::vector<glm::vec4> mesh_vert_pos;
    std::vector<glm::vec4> mesh_vert_nor;
    std::vector<glm::vec4> mesh_vert_col;
    std::vector<glm::ivec2> mesh_vert_jt;
    std::vector<glm::vec2> mesh_vert_inf;

    setup(faces, mesh_idx, mesh_vert_pos, mesh_vert_nor, mesh_vert_col, mesh_vert_jt, mesh_vert_inf);

    count = mesh_idx.size();

    // Create a VBO on our GPU and store its handle in bufIdx
    generateIdx();
    // Tell OpenGL that we want to perform subsequent operations on the VBO referred to by bufIdx
    // and that it will be treated as an element array buffer (since it will contain triangle indices)
    mp_context->glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, bufIdx);
    // Pass the data stored in cyl_idx into the bound buffer, reading a number of bytes equal to
    // CYL_IDX_COUNT multiplied by the size of a GLuint. This data is sent to the GPU to be read by shader programs.
    mp_context->glBufferData(GL_ELEMENT_ARRAY_BUFFER, mesh_idx.size() * sizeof(GLuint), mesh_idx.data(), GL_STATIC_DRAW);

    // The next few sets of function calls are basically the same as above, except bufPos and bufNor are
    // array buffers rather than element array buffers, as they store vertex attributes like position.
    generatePos();
    mp_context->glBindBuffer(GL_ARRAY_BUFFER, bufPos);
    mp_context->glBufferData(GL_ARRAY_BUFFER, mesh_vert_pos.size() * sizeof(glm::vec4), mesh_vert_pos.data(), GL_STATIC_DRAW);

    generateCol();
    mp_context->glBindBuffer(GL_ARRAY_BUFFER, bufCol);
    mp_context->glBufferData(GL_ARRAY_BUFFER, mesh_vert_col.size() * sizeof(glm::vec4), mesh_vert_col.data(), GL_STATIC_DRAW);

    generateNor();
    mp_context->glBindBuffer(GL_ARRAY_BUFFER, bufNor);
    mp_context->glBufferData(GL_ARRAY_BUFFER, mesh_vert_nor.size() * sizeof(glm::vec4), mesh_vert_nor.data(), GL_STATIC_DRAW);

    generateJtID();
    mp_context->glBindBuffer(GL_ARRAY_BUFFER, bufJtID);
    mp_context->glBufferData(GL_ARRAY_BUFFER, mesh_vert_jt.size() * sizeof(glm::ivec2), mesh_vert_jt.data(), GL_STATIC_DRAW);

    generateJtInf();
    mp_context->glBindBuffer(GL_ARRAY_BUFFER, bufJtInf);
    mp_context->glBufferData(GL_ARRAY_BUFFER, mesh_vert_inf.size() * sizeof(glm::vec2), mesh_vert_inf.data(), GL_STATIC_DRAW);
}



