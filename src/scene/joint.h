#ifndef JOINT_H
#define JOINT_H

#include <QString>
#include <QTreeWidgetItem>
#include <la.h>
#include "drawable.h"

class Joint : public Drawable, public QTreeWidgetItem {

private:
    // the name of this joint which will be displayed in the QTreeWidget of joints
    QString name;

    // the joint that is the parent of this joint
    Joint* parent;

    // the set of joints that have this joint as their parent
    QSet<Joint*> children;

    // the position of this joint relative to its parent join
    glm::vec4 position;

    // the quaternion that represents this joint's current orientation
    glm::quat quaternion;

    // the inverse of the joint's compound transformation matrix at
    // the time a mesh is bound to the joint's skeleton
    glm::mat4 bindMatrix;

    // has the joint been selected in GUI?
    bool selected;

    // joint creation id
    int id;

public:
    // id counter
    static int idCount;

    // constructor
    Joint(QString str, Joint* prt, glm::vec4 pos, glm::quat quat, GLWidget277 *context);

    // add child to the set of children
    void addChild(Joint *child);

    // set joint position
    void setPosition(const glm::vec4 pos);

    // set bind matrix
    void setBindMatrix();

    // set quat
    void setQuat(const glm::quat quat);

    // is the joint selected?
    bool isSelected();

    // the joint has been selected
    void setSelected(const bool pressed);

    // getter for parent
    Joint* getParent() const;

    // getter for children
    QSet<Joint*> getChildren() const;

    // getter for position
    glm::vec4 getWorldPosition() const;
    glm::vec4 getLocalPosition() const;

    // getter for quaternion
    glm::quat getQuaternion() const;

    // get bind matrix
    glm::mat4 getBindMatrix() const;

    // get id
    int getID();

    // reset id
    void resetID();

    // returns a mat4 that represents the concatenation of a joint's position and rotation
    glm::mat4 getLocalTransformation() const;

    // mat4 that represents the concatentation of this joint's local transformation
    // with the transformations of its chain of parent joints
    glm::mat4 getOverallTransformation() const;

    // change draw mode
    virtual GLenum drawMode() override;

    // create joint drawing
    virtual void create() override;

};

#endif // JOINT_H
