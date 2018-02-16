#include "Interpolate.h"

Interpolate::Interpolate() :
    lineToFit(NULL),
    numComponents(0),
    drawSampleRate(1)
{

}

void Interpolate::draw() {
    draw(QVector3D(1.0,0.0,0.0));
}

void Interpolate::draw(QVector3D desiredColour) {

    if (lineToFit==NULL) {
        return;
    }

    glLineWidth(3.5);
    glColor3f(desiredColour.x(),
              desiredColour.y(),
              desiredColour.z());

    glBegin(GL_LINE_STRIP);

    for (int i=0;i<lineToFit->getNumComponents()-1;i++) {

        for (int j=0;j<=drawSampleRate;j++) {

            QVector3D samplePoint=interp(i,(float)j/(float)drawSampleRate);
            glVertex3f(samplePoint.x(),
                       samplePoint.y(),
                       samplePoint.z());

        }

    }
    glEnd();

    glColor3f(1.0,1.0,1.0);

}

QVector3D Interpolate::getTangent(int segment, double interpAmt) {

    QVector3D tangentVector=interp(segment,interpAmt+TANGENT_FINITEDIFFERENCE)-
            interp(segment,interpAmt-TANGENT_FINITEDIFFERENCE);

    return tangentVector;

}

QVector3D Interpolate::getLeftNormal(int segment, double interpAmt) {

    QVector3D tangentVec=getTangent(segment,interpAmt);
    QVector3D normalVec=QVector3D(tangentVec.y(),-tangentVec.x(),0);

    tangentVec = QVector3D::crossProduct(tangentVec,normalVec);
    normalVec.normalize();
    if (tangentVec.z()>0) {
        return normalVec;
    }
    else {
        return normalVec*(-1.0);
    }

}

QVector3D Interpolate::getRightNormal(int segment, double interpAmt) {
    return getLeftNormal(segment,interpAmt)*(-1.0);
}

