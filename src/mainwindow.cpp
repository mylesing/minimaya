#include "mainwindow.h"
#include <ui_mainwindow.h>
#include "cameracontrolshelp.h"
#include <scene/joint.h>


MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    ui->mygl->setFocus();
    // get clicked item and store it
    connect(ui->vertList, SIGNAL(itemClicked(QListWidgetItem*)), this, SLOT(slot_receiveVertClicked(QListWidgetItem*)));
    connect(ui->heList, SIGNAL(itemClicked(QListWidgetItem*)), this, SLOT(slot_receiveHEClicked(QListWidgetItem*)));
    connect(ui->faceList, SIGNAL(itemClicked(QListWidgetItem*)), this, SLOT(slot_receiveFaceClicked(QListWidgetItem*)));

    // send clicked item to change in mygl
    connect(this, SIGNAL(sendVertex(Vertex*)), ui->mygl, SLOT(slot_getCurrVertex(Vertex*)));
    connect(this, SIGNAL(sendHE(HalfEdge*)), ui->mygl, SLOT(slot_getCurrHE(HalfEdge*)));
    connect(this, SIGNAL(sendFace(Face*)), ui->mygl, SLOT(slot_getCurrFace(Face*)));

    // get position spinbox value
    connect(ui->changeX, SIGNAL(valueChanged(double)), this, SLOT(slot_newX(double)));
    connect(ui->changeY, SIGNAL(valueChanged(double)), this, SLOT(slot_newY(double)));
    connect(ui->changeZ, SIGNAL(valueChanged(double)), this, SLOT(slot_newZ(double)));

    // change position
    connect(this, SIGNAL(sendNewXPos(double)), ui->mygl, SLOT(slot_changeX(double)));
    connect(this, SIGNAL(sendNewYPos(double)), ui->mygl, SLOT(slot_changeY(double)));
    connect(this, SIGNAL(sendNewZPos(double)), ui->mygl, SLOT(slot_changeZ(double)));

    // get color spinbox value
    connect(ui->changeR, SIGNAL(valueChanged(double)), this, SLOT(slot_newR(double)));
    connect(ui->changeG, SIGNAL(valueChanged(double)), this, SLOT(slot_newG(double)));
    connect(ui->changeB, SIGNAL(valueChanged(double)), this, SLOT(slot_newB(double)));

    // change color
    connect(this, SIGNAL(sendNewR(double)), ui->mygl, SLOT(slot_changeR(double)));
    connect(this, SIGNAL(sendNewG(double)), ui->mygl, SLOT(slot_changeG(double)));
    connect(this, SIGNAL(sendNewB(double)), ui->mygl, SLOT(slot_changeB(double)));

    // add vertex to half-edge
    connect(ui->addVertButton, SIGNAL(clicked(bool)), this, SLOT(slot_addNewVertex(bool)));
    connect(this, SIGNAL(sendAddVertex(bool)), ui->mygl, SLOT(slot_addVertex(bool)));

    // triangulate face
    connect(ui->triangulateButton, SIGNAL(clicked(bool)), this, SLOT(slot_triangulateClicked(bool)));
    connect(this, SIGNAL(sendTriangulate(bool)), ui->mygl, SLOT(slot_triangulate(bool)));

    // subdivide mesh
    connect(ui->subdivideButton, SIGNAL(clicked(bool)), this, SLOT(slot_subdivideClicked(bool)));
    connect(this, SIGNAL(sendSubdivide(bool)), ui->mygl, SLOT(slot_subdivide(bool)));

    // extrude mesh
    connect(ui->extrudeButton, SIGNAL(clicked(bool)), this, SLOT(slot_extrudeClicked(bool)));
    connect(this, SIGNAL(sendExtrude(bool)), ui->mygl, SLOT(slot_extrude(bool)));

    // load OBJ
    connect(ui->loadOBJButton, SIGNAL(clicked(bool)), this, SLOT(slot_loadOBJClicked(bool)));
    connect(this, SIGNAL(sendLoadOBJ(bool)), ui->mygl, SLOT(slot_loadOBJ(bool)));

    // load skeleton
    connect(ui->loadSkeleton, SIGNAL(clicked(bool)), this, SLOT(slot_loadSkeletonClicked(bool)));
    connect(this, SIGNAL(sendLoadSkeleton(bool)), ui->mygl, SLOT(slot_loadSkeleton(bool)));

    // send joint root to create tree
    connect(ui->mygl, SIGNAL(sendRoot(Joint*)), this, SLOT(slot_storeRoot(Joint*)));

    // get selected joint
    connect(ui->treeWidget, SIGNAL(itemClicked(QTreeWidgetItem*,int)), this, SLOT(slot_receiveClickedJoint(QTreeWidgetItem*, int)));
    connect(this, SIGNAL(sendCurrJoint(Joint*)), ui->mygl, SLOT(slot_storeCurrJoint(Joint*)));

    // skin mesh
    connect(ui->skinMeshButton, SIGNAL(clicked(bool)), this, SLOT(slot_skinMeshClicked(bool)));
    connect(this, SIGNAL(sendSkinMeshClicked(bool)), ui->mygl, SLOT(slot_setVertexInfluence(bool)));

    // send current position
    connect(ui->mygl, SIGNAL(sendJointPos(glm::vec4)), this, SLOT(slot_setJtPos(glm::vec4)));
    connect(this, SIGNAL(sendSetJtPosX(double)), ui->jtPosX, SLOT(setValue(double)));
    connect(this, SIGNAL(sendSetJtPosY(double)), ui->jtPosY, SLOT(setValue(double)));
    connect(this, SIGNAL(sendSetJtPosZ(double)), ui->jtPosZ, SLOT(setValue(double)));

    // get joint position spinbox value
    connect(ui->jtPosX, SIGNAL(valueChanged(double)), this, SLOT(slot_newJtX(double)));
    connect(ui->jtPosY, SIGNAL(valueChanged(double)), this, SLOT(slot_newJtY(double)));
    connect(ui->jtPosZ, SIGNAL(valueChanged(double)), this, SLOT(slot_newJtZ(double)));

    // change position
    connect(this, SIGNAL(sendNewJtXPos(double)), ui->mygl, SLOT(slot_changeJtX(double)));
    connect(this, SIGNAL(sendNewJtYPos(double)), ui->mygl, SLOT(slot_changeJtY(double)));
    connect(this, SIGNAL(sendNewJtZPos(double)), ui->mygl, SLOT(slot_changeJtZ(double)));

    // get button pressed signal
    connect(ui->rotJtXButton, SIGNAL(clicked(bool)), this, SLOT(slot_rotateJtXClicked(bool)));
    connect(ui->rotJtYButton, SIGNAL(clicked(bool)), this, SLOT(slot_rotateJtYClicked(bool)));
    connect(ui->rotJtZButton, SIGNAL(clicked(bool)), this, SLOT(slot_rotateJtZClicked(bool)));

    // change rotation
    connect(this, SIGNAL(sendNewJtXRot(bool)), ui->mygl, SLOT(slot_changeJtXRot(bool)));
    connect(this, SIGNAL(sendNewJtYRot(bool)), ui->mygl, SLOT(slot_changeJtYRot(bool)));
    connect(this, SIGNAL(sendNewJtZRot(bool)), ui->mygl, SLOT(slot_changeJtZRot(bool)));

    // name items
    connect(ui->mygl, SIGNAL(sendVertex(Vertex*)), this, SLOT(slot_getVertex(Vertex*)));
    connect(ui->mygl, SIGNAL(sendHE(HalfEdge*)), this, SLOT(slot_getHE(HalfEdge*)));
    connect(ui->mygl, SIGNAL(sendFace(Face*)), this, SLOT(slot_getFace(Face*)));
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_actionQuit_triggered()
{
    QApplication::exit();
}

void MainWindow::on_actionCamera_Controls_triggered()
{
    CameraControlsHelp* c = new CameraControlsHelp();
    c->show();
}

// get mesh components and add them to the lists

void MainWindow::slot_getVertex(Vertex* v) {
    ui->vertList->addItem(v);
}

void MainWindow::slot_getHE(HalfEdge* he) {
    ui->heList->addItem(he);
}

void MainWindow::slot_getFace(Face* f) {
    ui->faceList->addItem(f);
}

// receive clicked mesh components

void MainWindow::slot_receiveVertClicked(QListWidgetItem* item) {
    Vertex* curr = (Vertex*) item;
    emit sendVertex(curr);
}

void MainWindow::slot_receiveHEClicked(QListWidgetItem* item) {
    HalfEdge* curr = (HalfEdge*) item;
    emit sendHE(curr);
}

void MainWindow::slot_receiveFaceClicked(QListWidgetItem* item) {
    Face* curr = (Face*) item;
    emit sendFace(curr);
}

// receive new position from spin box

void MainWindow::slot_newX(double x) {
    emit sendNewXPos(x);
}
void MainWindow::slot_newY(double y) {
    emit sendNewYPos(y);
}
void MainWindow::slot_newZ(double z) {
    emit sendNewYPos(z);
}

// receive new color from spin box

void MainWindow::slot_newR(double r) {
    emit sendNewR(r);
}
void MainWindow::slot_newG(double g) {
    emit sendNewG(g);
}

void MainWindow::slot_newB(double b) {
    emit sendNewB(b);
}

// add new vertex signal
void MainWindow::slot_addNewVertex(bool) {
    emit sendAddVertex(true);
}

// triangulate signal
void MainWindow::slot_triangulateClicked(bool) {
    emit sendTriangulate(true);
}

// subdivide signal
void MainWindow::slot_subdivideClicked(bool) {
    emit sendSubdivide(true);
}

// extrude signal
void MainWindow::slot_extrudeClicked(bool) {
    emit sendExtrude(true);
}

// load OBJ signal
void MainWindow::slot_loadOBJClicked(bool) {
    ui->vertList->clear();
    ui->heList->clear();
    ui->faceList->clear();
    emit sendLoadOBJ(true);
}

// load skeleton signal
void MainWindow::slot_loadSkeletonClicked(bool) {
    emit sendLoadSkeleton(true);
}

// get root joint from MyGL
void MainWindow::slot_storeRoot(Joint* jt) {
    ui->treeWidget->addTopLevelItem(jt);
}

// send the clicked joint to MyGL for selectiom
void MainWindow::slot_receiveClickedJoint(QTreeWidgetItem* item, int) {
    Joint* currJoint = (Joint*) item;
    emit sendCurrJoint(currJoint);
}

// skin mesh signal
void MainWindow::slot_skinMeshClicked(bool) {
    emit sendSkinMeshClicked(true);
}

// receive new position from spin box

void MainWindow::slot_newJtX(double x) {
    emit sendNewJtXPos(x);
}
void MainWindow::slot_newJtY(double y) {
    emit sendNewJtYPos(y);
}
void MainWindow::slot_newJtZ(double z) {
    emit sendNewJtZPos(z);
}

// skin mesh signal
void MainWindow::slot_setJtPos(glm::vec4 pos) {
    emit sendSetJtPosX((double) pos[0]);
    emit sendSetJtPosY((double) pos[1]);
    emit sendSetJtPosZ((double) pos[2]);
}

void MainWindow::slot_rotateJtXClicked(bool) {
    emit sendNewJtXRot(true);
}

void MainWindow::slot_rotateJtYClicked(bool) {
    emit sendNewJtYRot(true);
}

void MainWindow::slot_rotateJtZClicked(bool) {
    emit sendNewJtZRot(true);
}
