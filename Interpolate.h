/*
 * Interpolate.h - header file for abstract Interpolate class
 */

#ifndef INTERPOLATE_H
#define INTERPOLATE_H

#include "Line.h"
#include "OBJObject.h"
#include "cmath"

#define DEFAULT_DRAWSAMPLERATE 10
#define TANGENT_FINITEDIFFERENCE 0.01

class Interpolate : public QObject {
    Q_OBJECT

public:

    Interpolate();

	void draw();
    void draw(QVector3D desiredColour);
    QVector3D getTangent(int segment, double interpAmt);
    QVector3D getLeftNormal(int segment, double interpAmt);
    QVector3D getRightNormal(int segment, double interpAmt);

	virtual float findYgivenX(int segment, float desiredx) = 0;
    virtual QVector3D interp(int segment, double interpAmt) = 0;

protected:

	Line *lineToFit;
	int numComponents;
    QVector <QVector3D> tangentList;
	int drawSampleRate;

};

#endif
