#include "glwidget.h"

GLWidget::GLWidget() :
    rotate_x(0.0f),
    rotate_y(0.0f),
    rotate_z(0.0f),
    freeze_rotate_y(0.0f),
    freeze_trans_z(0.0f),
    m(NULL)
{       
    QSurfaceFormat format;
    format.setSamples(4); //<== widget show black screen if samples =4, 1 is ok but not antialiasing
    setFormat(format);

    QFormLayout * v = new QFormLayout();

    w = new QWidget();
    w->setLayout(v);

    /* Side UI */

    posZSpinner=new QDoubleSpinBox();
//    v->addRow(new QLabel("Translate (Z)"), posZSpinner);

    rotSpeedSpinnerY=new QDoubleSpinBox();
    rotSpeedSpinnerY->setRange(-10.0,10.0);
    rotSpeedSpinnerY->setValue(0.5);
    rotSpeedSpinnerY->setSingleStep(0.5);

    v->addRow(new QLabel("Rotation Speed (Y)"), rotSpeedSpinnerY);

    freezeTransformationCheckbox = new QCheckBox("Freeze Transformation");
    v->addRow(freezeTransformationCheckbox);

    stopRotatingButton = new QPushButton("Stop Rotating");
    connect(stopRotatingButton, SIGNAL(clicked(bool)), this, SLOT(StopRotating()));

    resetRotationsButton = new QPushButton("Reset Rotations");
    connect(resetRotationsButton, SIGNAL(clicked(bool)), this, SLOT(ResetRotations()));
    v->addRow(stopRotatingButton, resetRotationsButton);

    drawWireframeOnly = new QCheckBox("Draw Mesh Wireframe");
    drawMeshBoundaries = new QCheckBox("Draw Mesh Boundaries");
    v->addRow(drawWireframeOnly, drawMeshBoundaries);

    drawSolidMesh = new QCheckBox("Draw Mesh Triangles");
    drawSolidMesh->setChecked(true);
    drawMeshNormals = new QCheckBox("Draw Mesh Normals");
    v->addRow(drawSolidMesh, drawMeshNormals);

    scaleNormalsSpinner = new QDoubleSpinBox();
    scaleNormalsSpinner->setRange(0.01,40.0);
    scaleNormalsSpinner->setValue(1.0);
    scaleNormalsSpinner->setSingleStep(1);
    v->addRow(new QLabel("Scale Normals"), scaleNormalsSpinner);

    drawMeshEdgeContours = new QCheckBox("Draw Mesh Edge Contours");
    drawSmoothContours = new QCheckBox("Draw Smooth Contours");
    drawSmoothContours->setChecked(true);
    v->addRow(drawMeshEdgeContours, drawSmoothContours);

    drawKappa1 = new QCheckBox("Draw Kappa_1");
    drawKappa2 = new QCheckBox("Draw Kappa_2");
    v->addRow(drawKappa1, drawKappa2);

    drawKappar = new QCheckBox();
    v->addRow(new QLabel("Draw Kappa_r"), drawKappar);

    principalCurvatureThresholdSpinner = new QDoubleSpinBox();
    principalCurvatureThresholdSpinner->setRange(0.0,2.0);
    principalCurvatureThresholdSpinner->setValue(2.0);
    principalCurvatureThresholdSpinner->setSingleStep(0.1);
    v->addRow(new QLabel("Maximum Curvature"), principalCurvatureThresholdSpinner);

    QGroupBox * sugConGB = new QGroupBox("Suggestive Contours");
    QFormLayout * sugConL = new QFormLayout();
    sugConGB->setLayout(sugConL);

    drawSuggestiveContours = new QCheckBox("Use Suggestive Contours");
    drawSuggestiveContours->setChecked(true);
    sugConL->addRow(drawSuggestiveContours);

    thetaCSpinner = new QDoubleSpinBox();
    thetaCSpinner->setRange(0,90.0);
    thetaCSpinner->setValue(0.0);
    thetaCSpinner->setSingleStep(5.0);
    sugConL->addRow("Theta_c (degrees)", thetaCSpinner);

    t_dSpinner = new QDoubleSpinBox();
    t_dSpinner->setRange(0,0.05);
    t_dSpinner->setValue(0.0);
    t_dSpinner->setSingleStep(1.0);
    sugConL->addRow("t_d", t_dSpinner);

    testDWKRPositive = new QCheckBox();
    testDWKRPositive->setChecked(true);
    sugConL->addRow("Test D_w*kappa_r > t_d", testDWKRPositive);

    v->addRow(sugConGB);

    QGroupBox * toonGB = new QGroupBox("Toon Shading");
    QFormLayout * toonL = new QFormLayout();
    toonGB->setLayout(toonL);

    drawToonShaded = new QCheckBox("Use Toon Shading");
    toonL->addRow(drawToonShaded);

    ndotVSpinner = new QDoubleSpinBox();
    ndotVSpinner->setRange(0,1);
    ndotVSpinner->setValue(0.66);
    ndotVSpinner->setSingleStep(0.02);
    toonL->addRow("n dot v Boundary", ndotVSpinner);

    v->addRow(toonGB);

    QGroupBox * ccGB = new QGroupBox("Contour Chaining");
    QFormLayout * ccL = new QFormLayout();
    ccGB->setLayout(ccL);

    useContourChains = new QCheckBox("Use Contour Chaining");
    ccL->addRow(useContourChains);

    minChainLengthSpinner = new QSpinBox();
    minChainLengthSpinner->setRange(1,100);
    minChainLengthSpinner->setValue(1);
    minChainLengthSpinner->setSingleStep(1);
    ccL->addRow("Minimum Segments for Chain", minChainLengthSpinner);

    reverseChaikinSpinner = new QSpinBox();
    reverseChaikinSpinner->setRange(0,5);
    reverseChaikinSpinner->setValue(1);
    reverseChaikinSpinner->setSingleStep(1);
    ccL->addRow("Reverse Chaikin Passes", reverseChaikinSpinner);

    distClosedRadioButton = new QRadioButton("Draw Closed/Open Chains");
    distClosedRadioButton->setChecked(true);

    catRomRadioButton = new QRadioButton("Draw Catmull-Rom Splines");

    ccL->addRow(distClosedRadioButton, catRomRadioButton);

    useBrushRadioButton = new QRadioButton("Draw Brush OBJ Along Spline");
    ccL->addRow(useBrushRadioButton);

    maxStrokeWidthSpinner = new QDoubleSpinBox();
    maxStrokeWidthSpinner->setRange(0,5);
    maxStrokeWidthSpinner->setValue(1);
    maxStrokeWidthSpinner->setSingleStep(1);
    ccL->addRow("Brush Width", maxStrokeWidthSpinner);

    v->addRow(ccGB);

    setMouseTracking(true);
}

void GLWidget::initializeGL()
{
    glClearColor( 1,1,1,1 );
    glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_POINT_SMOOTH);
    glEnable(GL_LINE_SMOOTH);
    glEnable(GL_POLYGON_SMOOTH);

    timer.setInterval(0);
    timer.start();
    connect(&timer, SIGNAL(timeout()), this, SLOT(repaint()));
}


void GLWidget::resizeGL(int w, int h)
{
    makeCurrent();

    const float aspect = float(w)/float(h);
    //set up perspective projection
    glViewport(0,0,w,h);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glFrustum(-aspect,aspect,-1,1,1,5000);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
}

void GLWidget::mouseMoveEvent(QMouseEvent * e)
{
    const int x = e->x();
    const int y = e->y();

    if ((e->buttons() & Qt::LeftButton) > 0) {

        rotSpeedSpinnerY->setValue(0.0);
        rotate_y += (float)(x-last_mouse_x);
        posZSpinner->setValue(posZSpinner->value()+posZSpinner->value()*(float)(last_mouse_y-y)/100.0);

    }

    last_mouse_x=x;
    last_mouse_y=y;
}

void GLWidget::paintGL()
{
    glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

    if (m!=NULL) {

        glColor3f(1,1,1);

        glPushMatrix();

        glTranslatef(0,0,-posZSpinner->value());

        rotate_y+=rotSpeedSpinnerY->value();
        glRotatef(rotate_y,0,1,0);

        //if contour this, contour that, change this
        if (!freezeTransformationCheckbox->isChecked()) {
            freeze_rotate_y=rotate_y;
            freeze_trans_z=-posZSpinner->value();
        }

        if (useContourChains->isChecked()) {
            contourChainGroup.resetGroup();
        }

        //first calculate face visibilities
        determineFaceVisibilities();

        glPolygonOffset(0.0f,0.0f);
        if (drawMeshEdgeContours->isChecked())
            drawMeshEdgeContoursFunc();
        if (drawSmoothContours->isChecked())
            drawSmoothContoursFunc();
        if (drawKappa1->isChecked()||drawKappa2->isChecked())
            drawKappa12Func();
        if (drawKappar->isChecked()) {
            calculateKappaRFunc();
            drawKappaRFunc();
        }
        if (drawSuggestiveContours->isChecked()) {
            calculateKappaRFunc();
            calculateGradKappaRFunc();
            drawVisibleSuggestiveContoursFunc();
        }
        if (drawMeshBoundaries->isChecked())
            drawMeshBoundariesFunc();
        if (drawWireframeOnly->isChecked()) {
            drawMeshAsWireframeOnlyFunc();
        }

        //draw contour chains (if necessary)
        if (useContourChains->isChecked()) {
            contourChainGroup.finishedAdding();
            if (distClosedRadioButton->isChecked())
                contourChainGroup.draw(minChainLengthSpinner->value());
            else if (catRomRadioButton->isChecked())
                contourChainGroup.drawAsSplines(minChainLengthSpinner->value(), maxStrokeWidthSpinner->value(), reverseChaikinSpinner->value());
            else if (useBrushRadioButton->isChecked() && !brushObj.isNull())
                contourChainGroup.drawAsSplineOBJ(minChainLengthSpinner->value(),brushObj, maxStrokeWidthSpinner->value(), reverseChaikinSpinner->value());
        }

        glPolygonOffset(0.0f,0.0f);
        if (drawSolidMesh->isChecked() && !drawToonShaded->isChecked()) {
            drawMeshAsTrianglesFunc();
        }
        if (drawToonShaded->isChecked())
            drawToonShadedFunc();

        if (drawMeshNormals->isChecked())
            drawMeshNormalsFunc();

        glPopMatrix();

    }
}


void GLWidget::determineFaceVisibilities()
{
    faceVisible=QVector<int>(m->faces.size(), 0);

    for (unsigned int i=0;i<m->faces.size();i++) {

        QVector3D v1=QVector3D(m->vertices[m->faces[i][0]][0],m->vertices[m->faces[i][0]][1],m->vertices[m->faces[i][0]][2]);
        QVector3D v2=QVector3D(m->vertices[m->faces[i][1]][0],m->vertices[m->faces[i][1]][1],m->vertices[m->faces[i][1]][2]);
        QVector3D v3=QVector3D(m->vertices[m->faces[i][2]][0],m->vertices[m->faces[i][2]][1],m->vertices[m->faces[i][2]][2]);
        QVector3D n1=QVector3D(m->normals[m->faces[i][0]][0],m->normals[m->faces[i][0]][1],m->normals[m->faces[i][0]][2]);
        QVector3D n2=QVector3D(m->normals[m->faces[i][1]][0],m->normals[m->faces[i][1]][1],m->normals[m->faces[i][1]][2]);
        QVector3D n3=QVector3D(m->normals[m->faces[i][2]][0],m->normals[m->faces[i][2]][1],m->normals[m->faces[i][2]][2]);

        //perspective: where v(p)=c-p
        QVector3D t_v1=QVector3D(0,0,0)-transformVertex(v1,freeze_trans_z,freeze_rotate_y);
        QVector3D t_v2=QVector3D(0,0,0)-transformVertex(v2,freeze_trans_z,freeze_rotate_y);
        QVector3D t_v3=QVector3D(0,0,0)-transformVertex(v3,freeze_trans_z,freeze_rotate_y);

        t_v1.normalize();
        t_v2.normalize();
        t_v3.normalize();

        QVector3D t_n1=transformNormal(n1, freeze_rotate_y);
        QVector3D t_n2=transformNormal(n2, freeze_rotate_y);
        QVector3D t_n3=transformNormal(n3, freeze_rotate_y);
        t_n1.normalize();
        t_n2.normalize();
        t_n3.normalize();

        const float dot1=QVector3D::dotProduct(t_v1, t_n1);
        const float dot2=QVector3D::dotProduct(t_v2, t_n2);
        const float dot3=QVector3D::dotProduct(t_v3, t_n3);

        if (dot1>=0.0&&dot2>=0.0&&dot3>=0.0)
            faceVisible[i]=0;
        else if (dot1<0.0&&dot2<0.0&&dot3<0.0)
            faceVisible[i]=2;
        else
            faceVisible[i]=1;

    }
}

void GLWidget::drawMeshAsWireframeOnlyFunc() {
    glLineWidth(1.0);
    glColor3f(.7,.7,.8);

    for (unsigned int i=0;i<m->faces.size();i++) {
        if (faceVisible[i]<2) {
            glBegin(GL_LINE_LOOP);
            for (int j=0;j<3;j++)
                glVertex3f(m->vertices[m->faces[i][j]][0],m->vertices[m->faces[i][j]][1],m->vertices[m->faces[i][j]][2]);
            glEnd();
        }
    }
}

void GLWidget::drawMeshBoundariesFunc()
{
    for (unsigned int i=0;i<m->faces.size(); i++) {
        for (int j=0;j<3;j++) {
            if (m->across_edge[i][j] < 0) {
                QVector3D p1 = QVector3D(m->vertices[m->faces[i][(j+1)%3]][0],
                        m->vertices[m->faces[i][(j+1)%3]][1],
                        m->vertices[m->faces[i][(j+1)%3]][2]);
                QVector3D p2 = QVector3D(m->vertices[m->faces[i][(j+2)%3]][0],
                        m->vertices[m->faces[i][(j+2)%3]][1],
                        m->vertices[m->faces[i][(j+2)%3]][2]);
                drawGoodContourLine(p1,p2);
            }
        }
    }

}

void GLWidget::drawMeshAsTrianglesFunc()
{
    glLineWidth(1.0);
    glColor3f(1,1,1);

    glBegin(GL_TRIANGLES);
    for (unsigned int i=0;i<m->faces.size();i++) {
        if (faceVisible[i]<2) {
            for (int j=0;j<3;j++) {
                glVertex3f(m->vertices[m->faces[i][j]][0],m->vertices[m->faces[i][j]][1],m->vertices[m->faces[i][j]][2]);
            }
        }
    }
    glEnd();
}

void GLWidget::drawMeshNormalsFunc()
{
    glLineWidth(2.0);
    glColor3f(0,0,0);

    glBegin(GL_LINES);
    for (unsigned int i=0;i<m->vertices.size();i++) {
        glVertex3f(m->vertices[i][0],m->vertices[i][1],m->vertices[i][2]);
        glVertex3f(m->vertices[i][0]+m->normals[i][0]/scaleNormalsSpinner->value(),
                m->vertices[i][1]+m->normals[i][1]/scaleNormalsSpinner->value(),
                m->vertices[i][2]+m->normals[i][2]/scaleNormalsSpinner->value());
    }
    glEnd();
}

void GLWidget::drawMeshEdgeContoursFunc()
{
    glLineWidth(3.0);
    glColor3f(1,.2,.2);

    for (unsigned int i=0;i<m->faces.size();i++) {
        if (faceVisible[i]==1) { //specifically the "borderline" case
            QVector3D v1=QVector3D(m->vertices[m->faces[i][0]][0],m->vertices[m->faces[i][0]][1],m->vertices[m->faces[i][0]][2]);
            QVector3D v2=QVector3D(m->vertices[m->faces[i][1]][0],m->vertices[m->faces[i][1]][1],m->vertices[m->faces[i][1]][2]);
            QVector3D v3=QVector3D(m->vertices[m->faces[i][2]][0],m->vertices[m->faces[i][2]][1],m->vertices[m->faces[i][2]][2]);
            QVector3D n1=QVector3D(m->normals[m->faces[i][0]][0],m->normals[m->faces[i][0]][1],m->normals[m->faces[i][0]][2]);
            QVector3D n2=QVector3D(m->normals[m->faces[i][1]][0],m->normals[m->faces[i][1]][1],m->normals[m->faces[i][1]][2]);
            QVector3D n3=QVector3D(m->normals[m->faces[i][2]][0],m->normals[m->faces[i][2]][1],m->normals[m->faces[i][2]][2]);
            //perspective: where v(p)=c-p
            QVector3D t_v1=QVector3D(0,0,0)-transformVertex(v1,freeze_trans_z,freeze_rotate_y);
            QVector3D t_v2=QVector3D(0,0,0)-transformVertex(v2,freeze_trans_z,freeze_rotate_y);
            QVector3D t_v3=QVector3D(0,0,0)-transformVertex(v3,freeze_trans_z,freeze_rotate_y);

            t_v1.normalize();
            t_v2.normalize();
            t_v3.normalize();

            QVector3D t_n1=transformNormal(n1, freeze_rotate_y);
            QVector3D t_n2=transformNormal(n2, freeze_rotate_y);
            QVector3D t_n3=transformNormal(n3, freeze_rotate_y);
            t_n1.normalize();
            t_n2.normalize();
            t_n3.normalize();

            const float dot1=QVector3D::dotProduct(t_v1, t_n1);
            const float dot2=QVector3D::dotProduct(t_v2, t_n2);
            const float dot3=QVector3D::dotProduct(t_v3, t_n3);

            if (dot1*dot2<0.0) {
                glBegin(GL_LINES);
                glVertex3f(v1.x(),v1.y(),v1.z());
                glVertex3f(v2.x(),v2.y(),v2.z());
                glEnd();
            }
            if (dot1*dot3<0.0) {
                glBegin(GL_LINES);
                glVertex3f(v1.x(),v1.y(),v1.z());
                glVertex3f(v3.x(),v3.y(),v3.z());
                glEnd();
            }
            if (dot2*dot3<0.0) {
                glBegin(GL_LINES);
                glVertex3f(v2.x(),v2.y(),v2.z());
                glVertex3f(v3.x(),v3.y(),v3.z());
                glEnd();
            }
        }
    }
}

void GLWidget::drawGoodContourLine(const QVector3D & p1, const QVector3D & p2)
{
    glLineWidth(4.0);
    glColor3f(0,0,0);
    glPointSize(4.0);

    glBegin(GL_LINES);
    glVertex3f(p1.x(),p1.y(),p1.z());
    glVertex3f(p2.x(),p2.y(),p2.z());
    glEnd();
}

void GLWidget::drawSmoothContoursFunc() {

    QVector <bool> faceProcessed(m->faces.size());
    QList <int> faceToProcess;
    for (unsigned int i=0;i<m->faces.size();i++) {
        if (faceVisible[i]==1) {
            faceProcessed[i]=false;
            faceToProcess.push_back(i);
        }
        else
            faceProcessed[i]=true;
    }

    while (!faceToProcess.empty()) {

        int faceIndex=faceToProcess.front();
        faceToProcess.pop_front();

        if (faceProcessed[faceIndex]) continue;

        QVector3D v1=QVector3D(m->vertices[m->faces[faceIndex][0]][0],m->vertices[m->faces[faceIndex][0]][1],m->vertices[m->faces[faceIndex][0]][2]);
        QVector3D v2=QVector3D(m->vertices[m->faces[faceIndex][1]][0],m->vertices[m->faces[faceIndex][1]][1],m->vertices[m->faces[faceIndex][1]][2]);
        QVector3D v3=QVector3D(m->vertices[m->faces[faceIndex][2]][0],m->vertices[m->faces[faceIndex][2]][1],m->vertices[m->faces[faceIndex][2]][2]);
        QVector3D n1=QVector3D(m->normals[m->faces[faceIndex][0]][0],m->normals[m->faces[faceIndex][0]][1],m->normals[m->faces[faceIndex][0]][2]);
        QVector3D n2=QVector3D(m->normals[m->faces[faceIndex][1]][0],m->normals[m->faces[faceIndex][1]][1],m->normals[m->faces[faceIndex][1]][2]);
        QVector3D n3=QVector3D(m->normals[m->faces[faceIndex][2]][0],m->normals[m->faces[faceIndex][2]][1],m->normals[m->faces[faceIndex][2]][2]);

        //perspective: where v(p)=c-p
        QVector3D t_v1=QVector3D(0,0,0)-transformVertex(v1,freeze_trans_z,freeze_rotate_y);
        QVector3D t_v2=QVector3D(0,0,0)-transformVertex(v2,freeze_trans_z,freeze_rotate_y);
        QVector3D t_v3=QVector3D(0,0,0)-transformVertex(v3,freeze_trans_z,freeze_rotate_y);

        t_v1.normalize();
        t_v2.normalize();
        t_v3.normalize();

        QVector3D t_n1=transformNormal(n1, freeze_rotate_y);
        QVector3D t_n2=transformNormal(n2, freeze_rotate_y);
        QVector3D t_n3=transformNormal(n3, freeze_rotate_y);
        t_n1.normalize();
        t_n2.normalize();
        t_n3.normalize();

        const float dot1=QVector3D::dotProduct(t_v1, t_n1);
        const float dot2=QVector3D::dotProduct(t_v2, t_n2);
        const float dot3=QVector3D::dotProduct(t_v3, t_n3);

        //draw mesh edge contour (in red, if needed) 2 out of 3 are 1 sign, 1's the other

        QVector3D p1;
        QVector3D p2;
        if (dot1>=0.0&&dot2>=0.0&&dot3<0.0) {
            p1=interpolateZeroPoint(v1, dot1, v3, dot3);
            p2=interpolateZeroPoint(v2, dot2, v3, dot3);
        }
        else if (dot1<0.0&&dot2<0.0&&dot3>=0.0) {
            p1=interpolateZeroPoint(v1, dot1, v3, dot3);
            p2=interpolateZeroPoint(v2, dot2, v3, dot3);
        }
        else if (dot2>=0.0&&dot3>=0.0&&dot1<0.0) {
            p1=interpolateZeroPoint(v1, dot1, v3, dot3);
            p2=interpolateZeroPoint(v1, dot1, v2, dot2);
        }
        else if (dot2<0.0&&dot3<0.0&&dot1>=0.0) {
            p1=interpolateZeroPoint(v1, dot1, v3, dot3);
            p2=interpolateZeroPoint(v1, dot1, v2, dot2);
        }
        else if (dot3>=0.0&&dot1>=0.0&&dot2<0.0) {
            p1=interpolateZeroPoint(v2, dot2, v1, dot1);
            p2=interpolateZeroPoint(v2, dot2, v3, dot3);
        }
        else if (dot3<0.0&&dot1<0.0&&dot2>=0.0) {
            p1=interpolateZeroPoint(v2, dot2, v1, dot1);
            p2=interpolateZeroPoint(v2, dot2, v3, dot3);
        }

        if (useContourChains->isChecked()) {
            contourChainGroup.addSegmentToGroup(p1, p2);

            //resort remainder of list such that there is a better ordering (for contour chaining step)
            for (int j=0;j<3;j++) {

                int adjacentFaceIndex=m->across_edge[faceIndex][j];
                if (adjacentFaceIndex>=0&&!faceProcessed[adjacentFaceIndex]) {
                    faceToProcess.push_front(adjacentFaceIndex);
                }
            }
        }
        else {
            drawGoodContourLine(p1,p2);
        }

        faceProcessed[faceIndex]=true;
    }
}

void GLWidget::drawKappa12Func()
{
    for (unsigned int i=0;i<m->faces.size();i++) {
        if (faceVisible[i]<2) {
            glBegin(GL_TRIANGLES);
            for (int j=0;j<3;j++) {
                const float eachCol1=fabs(m->curv1[m->faces[i][j]]);
                const float eachCol2=fabs(m->curv2[m->faces[i][j]]);
                //printf("c1 c2: %f %f\n",eachCol1,eachCol2);
                const float maxCurvature=principalCurvatureThresholdSpinner->value();
                if (drawKappa1->isChecked() && drawKappa2->isChecked())
                    glColor3f(eachCol1/maxCurvature,0,eachCol2/maxCurvature);
                else if (drawKappa1->isChecked())
                    glColor3f(eachCol1/maxCurvature,0,0);
                else if (drawKappa2->isChecked())
                    glColor3f(0,0,eachCol2/maxCurvature);
                glVertex3f(m->vertices[m->faces[i][j]][0],m->vertices[m->faces[i][j]][1],m->vertices[m->faces[i][j]][2]);
            }
            glEnd();

        }
    }

}

void GLWidget::calculateKappaRFunc()
{
    kappa_r=QVector <float> (m->vertices.size());
    for (unsigned int i=0;i<m->vertices.size();i++) {

        QVector3D eachVertex=QVector3D(m->vertices[i][0],m->vertices[i][1],m->vertices[i][2]);
        eachVertex=transformVertex(eachVertex,freeze_trans_z,-freeze_rotate_y);

        QVector3D viewVec=QVector3D(0,0,0)-eachVertex;
        viewVec=transformNormal(viewVec,-freeze_rotate_y);
        viewVec.normalize();

        //here, u and v are components of view vector, u along principal direction,
        //v in direction perpendicular to that on tangent plane
        //this corresponds to the "cos(phi)" defined in the paper
        const float u=QVector3D::dotProduct(viewVec, QVector3D(m->pdir1[i][0],m->pdir1[i][1],m->pdir1[i][2]));
        const float v=QVector3D::dotProduct(viewVec, QVector3D(m->pdir2[i][0],m->pdir2[i][1],m->pdir2[i][2]));

        kappa_r[i]=(m->curv1[i]*u*u+m->curv2[i]*v*v)/(u*u+v*v);
    }
}

void GLWidget::calculateGradKappaRFunc()
{
    dwk_r=QVector<float>(m->vertices.size());

    for (unsigned int i=0;i<m->vertices.size();i++) {

        QVector3D eachVertex=QVector3D(m->vertices[i][0],m->vertices[i][1],m->vertices[i][2]);
        eachVertex=transformVertex(eachVertex,freeze_trans_z,-freeze_rotate_y);

        QVector3D viewVec=QVector3D(0,0,0)-eachVertex;
        viewVec=transformNormal(viewVec,-freeze_rotate_y);
        viewVec.normalize();

        QVector3D eachNormal=QVector3D(m->normals[i][0],m->normals[i][1],m->normals[i][2]);
        const float ndotv = QVector3D::dotProduct(viewVec, eachNormal);

        const float u=QVector3D::dotProduct(viewVec, QVector3D(m->pdir1[i][0],m->pdir1[i][1],m->pdir1[i][2]));
        const float v=QVector3D::dotProduct(viewVec, QVector3D(m->pdir2[i][0],m->pdir2[i][1],m->pdir2[i][2]));

        dwk_r[i]=(u*u*(u*m->dcurv[i][0]+3.0*v*m->dcurv[i][1])+
                v*v*(3.0*u*m->dcurv[i][2]+v*m->dcurv[i][3]))/(u*u+v*v)
                -2.0*ndotv*sqr((m->curv2[i]-m->curv1[i])*u*v/(u*u+v*v));
    }
}

void GLWidget::drawKappaRFunc()
{
    for (unsigned int i=0;i<m->faces.size();i++) {
        if (faceVisible[i]<2) {
            glBegin(GL_TRIANGLES);
            for (int j=0;j<3;j++) {

                const float eachCol=kappa_r[m->faces[i][j]];
                const float maxCurvature=principalCurvatureThresholdSpinner->value();

                if (eachCol>0.0)
                    glColor3f(eachCol/maxCurvature,0,0);
                else
                    glColor3f(0,0,eachCol/maxCurvature);
                glVertex3f(m->vertices[m->faces[i][j]][0],m->vertices[m->faces[i][j]][1],m->vertices[m->faces[i][j]][2]);
            }
            glEnd();
        }
    }
}

void GLWidget::drawVisibleSuggestiveContoursFunc()
{
    //now that we have the curvatures,
    //we find the zero crossings of the DERIVATIVE of the kappa_r's with respect to w, and draw lines between them
    //for this, we can just use the algorithm from before, just instead of the dot product of n and v
    //its the zero crossing of dkappa_r/dw

    glLineWidth(3.0);

    QVector <bool> faceProcessed(m->faces.size());
    QList <int> faceToProcess;

    for (unsigned int i=0;i<m->faces.size();i++) {
        if (faceVisible[i]<2) {

            if (!((kappa_r[m->faces[i][0]]>0.0&&kappa_r[m->faces[i][1]]>0.0&&kappa_r[m->faces[i][2]]>0.0)
                  ||(kappa_r[m->faces[i][0]]<0.0&&kappa_r[m->faces[i][1]]<0.0&&kappa_r[m->faces[i][2]]<0.0))) {
                faceProcessed[i]=false;
                faceToProcess.push_back(i);
            }
            else
                faceProcessed[i]=true;
        }
        else
            faceProcessed[i]=true;
    }

    while (!(faceToProcess.empty())) {

        int faceIndex=faceToProcess.front();
        faceToProcess.pop_front();

        if (faceProcessed[faceIndex]) continue;

        QVector3D v1=QVector3D(m->vertices[m->faces[faceIndex][0]][0],m->vertices[m->faces[faceIndex][0]][1],m->vertices[m->faces[faceIndex][0]][2]);
        QVector3D v2=QVector3D(m->vertices[m->faces[faceIndex][1]][0],m->vertices[m->faces[faceIndex][1]][1],m->vertices[m->faces[faceIndex][1]][2]);
        QVector3D v3=QVector3D(m->vertices[m->faces[faceIndex][2]][0],m->vertices[m->faces[faceIndex][2]][1],m->vertices[m->faces[faceIndex][2]][2]);

        const float dot1=kappa_r[m->faces[faceIndex][0]];
        const float dot2=kappa_r[m->faces[faceIndex][1]];
        const float dot3=kappa_r[m->faces[faceIndex][2]];

        //draw suggestive contour (in red, if needed) 2 out of 3 are 1 sign, 1's the other

        //HERE WE CHECK TO MAKE SURE THAT D_w * k_r > 0
        //or another way to put it is that grad(k_r) * w > 0 for each of the vertices!

        //in fact, we check t_d < grad(k_r) DOT w / || w ||
        bool allArePositive=true;

        if (testDWKRPositive->isChecked()) {
            for (int j=0;j<3;j++) {

                QVector3D eachVertex=QVector3D(m->vertices[m->faces[faceIndex][j]][0],m->vertices[m->faces[faceIndex][j]][1],m->vertices[m->faces[faceIndex][j]][2]);
                QVector3D eachNormal=QVector3D(m->normals[m->faces[faceIndex][j]][0],m->normals[m->faces[faceIndex][j]][1],m->normals[m->faces[faceIndex][j]][2]);

                QVector3D viewVec=QVector3D(0,0,0)-transformVertex(eachVertex,freeze_trans_z,freeze_rotate_y); //view vector uses transformed coordinate!!
                QVector3D normalProj=transformNormal(eachNormal,freeze_rotate_y); //normal given is transformed one!!
                const float len = QVector3D::dotProduct(normalProj, viewVec);
                normalProj = normalProj.normalized() * len;
                QVector3D w=viewVec-normalProj;

                if (dwk_r[m->faces[faceIndex][j]]/w.length()<t_dSpinner->value()) {
                    allArePositive=false;
                    break;
                }
            }
        }

        if (!allArePositive) {
            faceProcessed[faceIndex]=true;
            continue;
        }

        //HERE WE CHECK THAT theta_c < n(p) dot v(p) / || v(p) ||
        bool thetaCIsLess=true;

        for (int j=0;j<3;j++) {
            QVector3D eachVertex=QVector3D(m->vertices[m->faces[faceIndex][j]][0],m->vertices[m->faces[faceIndex][j]][1],m->vertices[m->faces[faceIndex][j]][2]);
            QVector3D eachNormal=QVector3D(m->normals[m->faces[faceIndex][j]][0],m->normals[m->faces[faceIndex][j]][1],m->normals[m->faces[faceIndex][j]][2]);

            QVector3D viewVec=QVector3D(0,0,0)-transformVertex(eachVertex,freeze_trans_z,freeze_rotate_y); //view vector uses transformed coordinate!!
            QVector3D eachNormalTransformed=transformNormal(eachNormal,freeze_rotate_y); //normal given is transformed one!!

            const float dp = QVector3D::dotProduct(eachNormalTransformed, viewVec);
            if (!(
                        (thetaCSpinner->value()*3.14159f/180.0f)<acosf(dp)/viewVec.length()

                    )) {
                thetaCIsLess=false;
                break;
            }
        }

        if (!thetaCIsLess) {
            faceProcessed[faceIndex]=true;
            continue;
        }

        //these have to pruned according to the formula D_w*k(r) > 0
        QVector3D p1;
        QVector3D p2;
        bool drawStroke=false;

        if (dot1>=0.0&&dot2>=0.0&&dot3<0.0) {
            p1=interpolateZeroPoint(v1, dot1, v3, dot3);
            p2=interpolateZeroPoint(v2, dot2, v3, dot3);
            drawStroke=true;
        }
        else if (dot1<0.0&&dot2<0.0&&dot3>=0.0) {
            p1=interpolateZeroPoint(v1, dot1, v3, dot3);
            p2=interpolateZeroPoint(v2, dot2, v3, dot3);
            drawStroke=true;
        }
        else if (dot2>=0.0&&dot3>=0.0&&dot1<0.0) {
            p1=interpolateZeroPoint(v1, dot1, v3, dot3);
            p2=interpolateZeroPoint(v1, dot1, v2, dot2);
            drawStroke=true;
        }
        else if (dot2<0.0&&dot3<0.0&&dot1>=0.0) {
            p1=interpolateZeroPoint(v1, dot1, v3, dot3);
            p2=interpolateZeroPoint(v1, dot1, v2, dot2);
            drawStroke=true;
        }
        else if (dot3>=0.0&&dot1>=0.0&&dot2<0.0) {
            p1=interpolateZeroPoint(v2, dot2, v1, dot1);
            p2=interpolateZeroPoint(v2, dot2, v3, dot3);
            drawStroke=true;
        }
        else if (dot3<0.0&&dot1<0.0&&dot2>=0.0) {
            p1=interpolateZeroPoint(v2, dot2, v1, dot1);
            p2=interpolateZeroPoint(v2, dot2, v3, dot3);
            drawStroke=true;
        }

        //draw stroke
        if (drawStroke) {
            if (useContourChains->isChecked()) {
                contourChainGroup.addSegmentToGroup(p1, p2);

                //if a stroke is drawn, do the reordering to prioritize neighbours for contour chaining
                for (int j=0;j<3;j++) {

                    int adjacentFaceIndex=m->across_edge[faceIndex][j];
                    if (adjacentFaceIndex>=0&&!faceProcessed[adjacentFaceIndex]) {
                        //faceToProcess.remove(adjacentFaceIndex);
                        faceToProcess.push_front(adjacentFaceIndex);
                    }
                }
            }
            else {
                drawGoodContourLine(p1,p2);
            }
        }

        faceProcessed[faceIndex]=true;
    }
}

void GLWidget::drawToonTriangle(QVector3D v1, QVector3D v2, QVector3D v3, float col) {

    glColor3f(col,col,col);
    glBegin(GL_TRIANGLES);
    glVertex3f(v1.x(),v1.y(),v1.z());
    glVertex3f(v2.x(),v2.y(),v2.z());
    glVertex3f(v3.x(),v3.y(),v3.z());
    glEnd();

}

void GLWidget::drawToonShadedFunc() {

    glDisable(GL_BLEND);

    for (unsigned int i=0;i<m->faces.size();i++) {

        if (faceVisible[i]<2) { //for visible faces

            QVector3D v1=QVector3D(m->vertices[m->faces[i][0]][0],m->vertices[m->faces[i][0]][1],m->vertices[m->faces[i][0]][2]);
            QVector3D v2=QVector3D(m->vertices[m->faces[i][1]][0],m->vertices[m->faces[i][1]][1],m->vertices[m->faces[i][1]][2]);
            QVector3D v3=QVector3D(m->vertices[m->faces[i][2]][0],m->vertices[m->faces[i][2]][1],m->vertices[m->faces[i][2]][2]);
            QVector3D n1=QVector3D(m->normals[m->faces[i][0]][0],m->normals[m->faces[i][0]][1],m->normals[m->faces[i][0]][2]);
            QVector3D n2=QVector3D(m->normals[m->faces[i][1]][0],m->normals[m->faces[i][1]][1],m->normals[m->faces[i][1]][2]);
            QVector3D n3=QVector3D(m->normals[m->faces[i][2]][0],m->normals[m->faces[i][2]][1],m->normals[m->faces[i][2]][2]);

            //perspective: where v(p)=c-p
            QVector3D t_v1=QVector3D(0,0,0)-transformVertex(v1,freeze_trans_z,freeze_rotate_y);
            QVector3D t_v2=QVector3D(0,0,0)-transformVertex(v2,freeze_trans_z,freeze_rotate_y);
            QVector3D t_v3=QVector3D(0,0,0)-transformVertex(v3,freeze_trans_z,freeze_rotate_y);

            t_v1.normalize();
            t_v2.normalize();
            t_v3.normalize();

            QVector3D t_n1=transformNormal(n1, freeze_rotate_y);
            QVector3D t_n2=transformNormal(n2, freeze_rotate_y);
            QVector3D t_n3=transformNormal(n3, freeze_rotate_y);
            t_n1.normalize();
            t_n2.normalize();
            t_n3.normalize();

            const float dot1=QVector3D::dotProduct(t_v1, t_n1);
            const float dot2=QVector3D::dotProduct(t_v2, t_n2);
            const float dot3=QVector3D::dotProduct(t_v3, t_n3);

            const float ndotvbound=ndotVSpinner->value();

            if (dot1>ndotvbound&&dot2>ndotvbound&&dot3>ndotvbound) {
                glColor3f(1.0,1.0,1.0);
                glBegin(GL_TRIANGLES);
                glVertex3f(v1.x(),v1.y(),v1.z());
                glVertex3f(v2.x(),v2.y(),v2.z());
                glVertex3f(v3.x(),v3.y(),v3.z());
                glEnd();
            }
            else if (dot1<ndotvbound&&dot2<ndotvbound&&dot3<ndotvbound) {
                glColor3f(.7,.7,.7);
                glBegin(GL_TRIANGLES);
                glVertex3f(v1.x(),v1.y(),v1.z());
                glVertex3f(v2.x(),v2.y(),v2.z());
                glVertex3f(v3.x(),v3.y(),v3.z());
                glEnd();
            }
            else {
                QVector3D p1;
                QVector3D p2;
                if (dot1>=ndotvbound&&dot2>=ndotvbound&&dot3<ndotvbound) {
                    p1=interpolateZeroPoint(v1, dot1-ndotvbound, v3, dot3-ndotvbound);
                    p2=interpolateZeroPoint(v2, dot2-ndotvbound, v3, dot3-ndotvbound);
                    drawToonTriangle(v3,p1,p2,.7);
                    drawToonTriangle(v1,p1,p2,1.0);
                    drawToonTriangle(v1,v2,p2,1.0);
                }
                else if (dot1<ndotvbound&&dot2<ndotvbound&&dot3>=ndotvbound) {
                    p1=interpolateZeroPoint(v1, dot1-ndotvbound, v3, dot3-ndotvbound);
                    p2=interpolateZeroPoint(v2, dot2-ndotvbound, v3, dot3-ndotvbound);
                    drawToonTriangle(v3,p1,p2,1.0);
                    drawToonTriangle(v1,p1,p2,.7);
                    drawToonTriangle(v1,v2,p2,.7);
                }
                else if (dot2>=ndotvbound&&dot3>=ndotvbound&&dot1<ndotvbound) {
                    p1=interpolateZeroPoint(v1, dot1-ndotvbound, v3, dot3-ndotvbound);
                    p2=interpolateZeroPoint(v1, dot1-ndotvbound, v2, dot2-ndotvbound);
                    drawToonTriangle(v1,p2,p1,.7);
                    drawToonTriangle(v2,p1,p2,1.0);
                    drawToonTriangle(v2,v3,p1,1.0);
                }
                else if (dot2<ndotvbound&&dot3<ndotvbound&&dot1>=ndotvbound) {
                    p1=interpolateZeroPoint(v1, dot1-ndotvbound, v3, dot3-ndotvbound);
                    p2=interpolateZeroPoint(v1, dot1-ndotvbound, v2, dot2-ndotvbound);
                    drawToonTriangle(v1,p2,p1,1.0);
                    drawToonTriangle(v2,p1,p2,.7);
                    drawToonTriangle(v2,v3,p1,.7);
                }
                else if (dot3>=ndotvbound&&dot1>=ndotvbound&&dot2<ndotvbound) {
                    p1=interpolateZeroPoint(v2, dot2-ndotvbound, v1, dot1-ndotvbound);
                    p2=interpolateZeroPoint(v2, dot2-ndotvbound, v3, dot3-ndotvbound);
                    drawToonTriangle(v2,p2,p1,.7);
                    drawToonTriangle(v1,p1,p2,1);
                    drawToonTriangle(v1,p2,v3,1);
                }
                else if (dot3<ndotvbound&&dot1<ndotvbound&&dot2>=ndotvbound) {
                    p1=interpolateZeroPoint(v2, dot2-ndotvbound, v1, dot1-ndotvbound);
                    p2=interpolateZeroPoint(v2, dot2-ndotvbound, v3, dot3-ndotvbound);
                    drawToonTriangle(v2,p2,p1,1);
                    drawToonTriangle(v1,p1,p2,.7);
                    drawToonTriangle(v1,p2,v3,.7);
                }
            }
        }
    }

    glEnable(GL_BLEND);
}

QVector3D GLWidget::transformVertex(QVector3D inputVec, float trans_z, float rot_y)
{
    QMatrix4x4 m;
    m.translate(0, 0, trans_z);
    m.rotate(rot_y, 0, 1, 0);
    return m.map(inputVec);
}

QVector3D GLWidget::transformNormal(QVector3D inputVec, float rot_y)
{
    QMatrix4x4 m;
    m.rotate(rot_y, 0, 1, 0);
    return m.mapVector(inputVec);
}

void GLWidget::LoadOBJ(const QString path)
{
    //load the mesh...
    m=TriMesh::read(path.toLatin1().data());
    if (!m) {
        m=NULL;
        qDebug() << "GLWidget::LoadOBJ() Couldn't load!" << path;
    }

    // Convert triangle strips to faces, if necessary
    m->need_faces();
    m->need_normals();
    m->need_bsphere();
    m->need_curvatures();
    m->need_dcurv();
    m->need_neighbors();
    m->need_adjacentfaces();
    m->need_across_edge();

    //set the translate z spinner right...
    posZSpinner->setRange(0,m->bsphere.r*4.0);
    posZSpinner->setValue(m->bsphere.r*2.0);
    posZSpinner->setSingleStep(0.5);

    t_dSpinner->setRange(0,1.0/sqrt(m->bsphere.r));
    t_dSpinner->setValue(0.0);
    t_dSpinner->setSingleStep(1.0);

    maxStrokeWidthSpinner->setRange(0,m->bsphere.r/10.0);
    maxStrokeWidthSpinner->setValue(m->bsphere.r/40.0);
    maxStrokeWidthSpinner->setSingleStep(1.0);

    //offset all vertices so theyre at the centre
    const float cx=m->bsphere.center[0];
    const float cy=m->bsphere.center[1];
    const float cz=m->bsphere.center[2];

    for (unsigned int i=0;i<m->vertices.size();i++) {
        m->vertices[i][0]=m->vertices[i][0]-cx;
        m->vertices[i][1]=m->vertices[i][1]-cy;
        m->vertices[i][2]=m->vertices[i][2]-cz;
    }
}

void GLWidget::ResetRotations()
{
    rotate_x=0.0;
    rotate_y=0.0;
    rotate_z=0.0;
}

void GLWidget::StopRotating()
{
    rotSpeedSpinnerY->setValue(0.0);
}

void GLWidget::LoadBrushOBJ(const QString path)
{
    if (brushObj) {
        delete brushObj;
    }

    brushObj=new OBJObject();
    if (!brushObj->load(path)) {
        brushObj=NULL;
    }
}

QVector3D GLWidget::interpolateZeroPoint(QVector3D v1, float val1, QVector3D v2, float val2)
{
    if (val2<val1) {
        qSwap(v1, v2);
        qSwap(val1, val2);
    }

    const float t=-val1/(val2-val1);
    //printf("t: %f\n",t);
    return v1*(1-t)+v2*t;
}
