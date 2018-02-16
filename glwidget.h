#ifndef GLWIDGET_H
#define GLWIDGET_H

#include <QtGui>
#include <QtOpenGL>

#include "ContourChainGroup.h"
#include "OBJObject.h"

#include "TriMesh.h"
#include "TriMesh_algo.h"

using namespace trimesh;

class GLWidget : public QOpenGLWidget
{
    Q_OBJECT

public:

    GLWidget();

    void mouseMoveEvent(QMouseEvent * e);

    void initializeGL();
    void resizeGL(int w, int h);
    void paintGL();

    void LoadOBJ(const QString path);
    void LoadBrushOBJ(const QString path);

    QWidget * w;

public slots:

    void StopRotating();
    void ResetRotations();

private:

    QVector3D transformVertex(QVector3D inputVec, float trans_z, float rot_y);
    QVector3D transformNormal(QVector3D inputVec, float rot_y);
    QVector3D interpolateZeroPoint(QVector3D v1, float val1, QVector3D v2, float val2);

    void drawGoodContourLine(const QVector3D & p1, const QVector3D & p2);
    void determineFaceVisibilities();
    void drawMeshAsWireframeOnlyFunc();
    void drawMeshBoundariesFunc();
    void drawMeshAsTrianglesFunc();
    void drawMeshNormalsFunc();
    void drawMeshEdgeContoursFunc();
    void drawSmoothContoursFunc();
    void drawKappa12Func();
    void calculateKappaRFunc();
    void drawKappaRFunc();
    void calculateGradKappaRFunc();
    void drawVisibleSuggestiveContoursFunc();
    void drawToonShadedFunc();
    void drawToonTriangle(QVector3D v1, QVector3D v2, QVector3D v3, float col);

    float rotate_x, rotate_y, rotate_z;
    float freeze_rotate_y;
    float freeze_trans_z;
    int last_mouse_x, last_mouse_y;

    QDoubleSpinBox *rotSpeedSpinnerY;
    QDoubleSpinBox *posZSpinner;

    QDoubleSpinBox *principalCurvatureThresholdSpinner;
    QDoubleSpinBox *scaleNormalsSpinner;
    QDoubleSpinBox *thetaCSpinner;
    QDoubleSpinBox *t_dSpinner;
    QCheckBox *drawToonShaded;
    QDoubleSpinBox *ndotVSpinner;
    QDoubleSpinBox *maxStrokeWidthSpinner;
    QSpinBox *reverseChaikinSpinner;

    QRadioButton * distClosedRadioButton;
    QRadioButton * catRomRadioButton;
    QRadioButton * useBrushRadioButton;

    QSpinBox *minChainLengthSpinner;

    QCheckBox * freezeTransformationCheckbox;
    QPushButton * stopRotatingButton;
    QPushButton * resetRotationsButton;
    QCheckBox * drawMeshBoundaries;
    QCheckBox * drawWireframeOnly;
    QCheckBox * drawSolidMesh;
    QCheckBox * drawMeshNormals;
    QCheckBox * drawMeshEdgeContours;
    QCheckBox * drawSmoothContours;
    QCheckBox * drawKappa1;
    QCheckBox * drawKappa2;
    QCheckBox * drawKappar;
    QCheckBox * drawSuggestiveContours;
    QCheckBox * testDWKRPositive;

    QCheckBox * useContourChains;
    QCheckBox * useContoursToDraw;

    //mesh is global
    QVector <int> faceVisible; //0 - visible, 1 - a contour face, 2 - not visible
    TriMesh * m;
    QVector <float> kappa_r;
    QVector3D gradkappa_r;
    QVector <float> dwk_r;
    ContourChainGroup contourChainGroup;
    QPointer <OBJObject> brushObj;

    QTimer timer;

};

#endif // GLWIDGET_H
