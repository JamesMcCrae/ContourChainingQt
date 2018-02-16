//#include "TriMesh.h"
//#include "TriMesh_algo.h"

#include <QApplication>
#include <QtOpenGL>
#include <QtGui> //replacement for GLUI

#include <iostream>

#include "mainwindow.h"

#define DEG2RAD(x) x*3.14159/180.0

#define DEFAULT_ROTATE_X 0.0
#define DEFAULT_ROTATE_Z 0.0
#define DEFAULT_FRAMERATE 15
#define DEFAULT_NUMBER_SECONDS 5

#define MESH_SMOOTHNESS 1.1

/********** User IDs for callbacks ********/
#define LOAD_OBJ_BUTTON_ID	 101
#define STOP_ROTATING_BUTTON_ID 103
#define RECORD_AVI_BUTTON_ID 104
#define RESET_ROTATIONS_BUTTON_ID 105
#define LOAD_BRUSH_OBJ_BUTTON_ID 106

int main(int argc, char *argv[])
{	

    /****************************************/
    /*   Initialize GLUT and create window  */
    /****************************************/

    QApplication a(argc, argv);

    MainWindow w;

    w.show();
    a.exec();

//    glutMainLoop();

}
