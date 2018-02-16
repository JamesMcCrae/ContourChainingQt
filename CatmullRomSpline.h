/*
 * CatmullRomSpline.h - header file for CatmullRomSpline class
 */

#ifndef CATMULLROMSPLINE_H
#define CATMULLROMSPLINE_H

#include "Interpolate.h"

class CatmullRomSpline: public Interpolate {

public:

    CatmullRomSpline(Line *lineToFit);

    QVector3D interp(int segment, double interpAmt);
	float findYgivenX(int segment, float desiredx);

	void drawWithObject(OBJObject *obj, float maxstrokewidth);
	void drawAsTexturedQuads(float maxstrokewidth);	

};

#endif 
