#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <scene/mesh.h>
#include <scene/vertex.h>
#include <scene/joint.h>

namespace Ui {
class MainWindow;
}


class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private slots:
    void on_actionQuit_triggered();

    void on_actionCamera_Controls_triggered();

    void slot_getVertex(Vertex*);
    void slot_getHE(HalfEdge*);
    void slot_getFace(Face*);

    void slot_receiveVertClicked(QListWidgetItem*);
    void slot_receiveHEClicked(QListWidgetItem*);
    void slot_receiveFaceClicked(QListWidgetItem*);

    void slot_newX(double);
    void slot_newY(double);
    void slot_newZ(double);

    void slot_newR(double);
    void slot_newG(double);
    void slot_newB(double);

    void slot_addNewVertex(bool);
    void slot_triangulateClicked(bool);
    void slot_subdivideClicked(bool);
    void slot_extrudeClicked(bool);

    void slot_loadOBJClicked(bool);

    void slot_loadSkeletonClicked(bool);

    void slot_storeRoot(Joint*);
    void slot_receiveClickedJoint(QTreeWidgetItem*, int);

    void slot_skinMeshClicked(bool);

    void slot_setJtPos(glm::vec4);

    void slot_newJtX(double);
    void slot_newJtY(double);
    void slot_newJtZ(double);

    void slot_rotateJtXClicked(bool);
    void slot_rotateJtYClicked(bool);
    void slot_rotateJtZClicked(bool);

signals:
    void sendVertex(Vertex*);
    void sendHE(HalfEdge*);
    void sendFace(Face*);

    void sendNewXPos(double);
    void sendNewYPos(double);
    void sendNewZPos(double);

    void sendNewR(double);
    void sendNewG(double);
    void sendNewB(double);

    void sendAddVertex(bool);
    void sendTriangulate(bool);
    void sendSubdivide(bool);
    void sendExtrude(bool);

    void sendLoadOBJ(bool);
    void sendLoadSkeleton(bool);

    void sendCurrJoint(Joint*);

    void sendSkinMeshClicked(bool);

    void sendSetJtPosX(double);
    void sendSetJtPosY(double);
    void sendSetJtPosZ(double);

    void sendNewJtXPos(double);
    void sendNewJtYPos(double);
    void sendNewJtZPos(double);

    void sendNewJtXRot(bool);
    void sendNewJtYRot(bool);
    void sendNewJtZRot(bool);

private:
    Ui::MainWindow *ui;
};


#endif // MAINWINDOW_H
