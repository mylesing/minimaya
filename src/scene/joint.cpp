#include "joint.h"
#include <la.h>
#include <glm/gtx/rotate_vector.hpp>

// id count
int Joint::idCount = 0;

// constructor
Joint::Joint(QString str, Joint* prt, glm::vec4 pos, glm::quat quat, GLWidget277* context) :
    name(str),
    parent(prt),
    position(pos),
    quaternion(quat),
    selected(false),
    id(idCount++),
    Drawable(context)
{
    setText(0, name);
}

void Joint::addChild(Joint* child) {
    QTreeWidgetItem::addChild(child);
    children.insert(child);
}

void Joint::setPosition(const glm::vec4 pos) {
    position = pos;
}

void Joint::setBindMatrix() {
    bindMatrix = glm::inverse(getOverallTransformation());
}

// set quat
void Joint::setQuat(const glm::quat quat) {
    quaternion = quat;
}

// getter for parent
Joint* Joint::getParent() const {
    return parent;
}

// getter for children
QSet<Joint*> Joint::getChildren() const {
    return children;
}

// getter for world position
glm::vec4 Joint::getWorldPosition() const {
    return getOverallTransformation() * glm::vec4(0, 0, 0, 1);
}

// getter for local position
glm::vec4 Joint::getLocalPosition() const {
    return position;
}

// getter for quaternion
glm::quat Joint::getQuaternion() const {
    return quaternion;
}

// get bind matrix
glm::mat4 Joint::getBindMatrix() const {
    return bindMatrix;
}

// is the joint selected?
bool Joint::isSelected() {
    return selected;
}

// the joint has been selected
void Joint::setSelected(bool pressed) {
    selected = pressed;
}

// id getter
int Joint::getID() {
    return id;
}

// reset id
void Joint::resetID() {
    idCount = 0;
}

// returns a mat4 that represents the concatenation of a joint's position and rotation
glm::mat4 Joint::getLocalTransformation() const {
    glm::mat4 pos = glm::mat4(glm::vec4(1, 0, 0, 0),
                              glm::vec4(0, 1, 0, 0),
                              glm::vec4(0, 0, 1, 0),
                              position);
    glm::mat4 rot = glm::mat4_cast(quaternion);
    pos = pos * rot;
    return pos;
}

// mat4 that represents the concatentation of this joint's local transformation
// with the transformations of its chain of parent joints
glm::mat4 Joint::getOverallTransformation() const {
    if (parent == nullptr) {
        return this->getLocalTransformation();
    }

    return (parent->getOverallTransformation() * this->getLocalTransformation());
}

// change draw mode
GLenum Joint::drawMode() {
    return GL_LINES;
}

// create joint drawing
void Joint::create() {
    std::vector<GLuint> idx;
    std::vector<glm::vec4> pos;
    std::vector<glm::vec4> col;

    // circle around X axis
    glm::vec4 startPosX = glm::vec4(0, 0.5, 0, 1);
    for (int i = 0; i < 20; ++i) {
        if (i == 19) {
            idx.push_back(i);
            idx.push_back(0);
        } else {
            idx.push_back(i);
            idx.push_back(i + 1);
        }
        glm::vec4 shift = glm::rotateX(startPosX, glm::radians(i * 18.0f));
        shift = getOverallTransformation() * shift;
        pos.push_back(shift);
        if (isSelected()) {
            col.push_back(glm::vec4(1, 1, 1, 1));
        } else {
            col.push_back(glm::vec4(1, 0, 0, 1));
        }
    }

    // circle around Y axis
    glm::vec4 startPosY = glm::vec4(0, 0, 0.5, 1);
    for (int i = 20; i < 40; ++i) {
        if (i == 39) {
            idx.push_back(i);
            idx.push_back(20);
        } else {
            idx.push_back(i);
            idx.push_back(i + 1);
        }
        glm::vec4 shift = glm::rotateY(startPosY, glm::radians(i * 18.0f));
        shift = getOverallTransformation() * shift;
        pos.push_back(shift);
        if (isSelected()) {
            col.push_back(glm::vec4(1, 1, 1, 1));
        } else {
            col.push_back(glm::vec4(0, 1, 0, 1));
        }
    }

    // circle around Z axis
    glm::vec4 startPosZ = glm::vec4(0.5, 0, 0, 1);
    for (int i = 40; i < 60; ++i) {
        if (i == 59) {
            idx.push_back(i);
            idx.push_back(40);
        } else {
            idx.push_back(i);
            idx.push_back(i + 1);
        }
        glm::vec4 shift = glm::rotateZ(startPosZ, glm::radians(i * 18.0f));
        shift = getOverallTransformation() * shift;
        pos.push_back(shift);
        if (isSelected()) {
            col.push_back(glm::vec4(1, 1, 1, 1));
        } else {
            col.push_back(glm::vec4(0, 0, 1, 1));
        }

    }

    if (parent != nullptr) {
        // draw connection to parent
        idx.push_back(60);
        idx.push_back(61);
        pos.push_back(getWorldPosition());
        pos.push_back(parent->getWorldPosition());
        col.push_back(glm::vec4(1, 1, 0, 1));
        col.push_back(glm::vec4(1, 0, 1, 1));
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
