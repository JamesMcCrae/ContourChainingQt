#include "ContourChain.h"

ContourChain::ContourChain() {

	forceClosed=false;

}

void ContourChain::forceChainClosed() {
	forceClosed=true;
}

bool ContourChain::connectsToChain(const QVector3D & v1, const QVector3D & v2) {

	if (contourChain.size()==0) { //if its empty, make this the first segment		
		return true;
	}
	else {
        const QVector3D start=contourChain.front();
        const QVector3D end=contourChain.back();
        const float scale=(v1-v2).length();

		if (isCloseEnough(v1,start,scale)||
			isCloseEnough(v2,start,scale)||
			isCloseEnough(v1,end,scale)||
			isCloseEnough(v2,end,scale)) {
			return true;
		}
	}

	return false;
	
}

bool ContourChain::isCloseEnough(const QVector3D & v1, const QVector3D & v2, float scale)
{
    return ((v1-v2).lengthSquared()<scale*CHAIN_SEGMENT_CLOSENESS_THRESHOLD);
}

void ContourChain::addSegmentToChain(const QVector3D & v1, const QVector3D & v2)
{
    if (contourChain.isEmpty()) {
		contourChain.push_back(v1);
		contourChain.push_back(v2);		
	}
	else {
        const QVector3D start=contourChain.front();
        const QVector3D end=contourChain.back();
        const float scale=(v1-v2).length();

		if (isCloseEnough(v1,start,scale)) {
			contourChain.push_front(v2);
		}
		else if (isCloseEnough(v2,start,scale)) {
			contourChain.push_front(v1);
		}
		else if (isCloseEnough(v1,end,scale)) {
			contourChain.push_back(v2);
		}
		else if (isCloseEnough(v2,end,scale)) {
			contourChain.push_back(v1);
		}
	}

}

int ContourChain::getLength()
{
	return contourChain.size();
}

bool ContourChain::isChainClosed()
{
    if (forceClosed) {
        return true;
    }

	if (contourChain.size()>0&&isCloseEnough(contourChain.front(),contourChain.back(),1.0)) 
		return true;
	else
		return false;

}

void ContourChain::draw() {

	if (isChainClosed()&&!forceClosed) {
		glColor3f(0,1,0);	
		glBegin(GL_LINE_LOOP);
	}
	else {
		glColor3f(1,0,0);
		glBegin(GL_LINE_STRIP);
	}
	
	float curIndex=0.0;

    for (QVector3D & v : contourChain) {
		if (forceClosed) {
			glColor3f(curIndex/((float)contourChain.size()),0,0);		
			curIndex+=1.0;
		}
        glVertex3f(v.x(), v.y(), v.z());
	}
	glEnd();

}

void ContourChain::reset() {

	contourChain.clear();

}

void ContourChain::drawAsSpline(int numReverseChaikinPasses) {

    QPointer <Line> line=new Line();
    QPointer <Line> filteredLine;

    for (QVector3D & v : contourChain) {
        line->addComponentAtEnd(v);
	}

	for (int i=0;i<numReverseChaikinPasses;i++) {
		filteredLine=line->reverseChaikin();
		delete line;
		line=filteredLine;
	}

    QPointer <CatmullRomSpline> spline = new CatmullRomSpline(line);
    spline->draw(QVector3D());

	delete spline;
	delete line;

}

void ContourChain::drawAsSplineOBJ(OBJObject *obj, float maxstrokewidth, int numReverseChaikinPasses) {

	Line *line=new Line();

    for (QVector3D & v : contourChain) {
        line->addComponentAtEnd(v);
	}

	Line *filteredLine;
	for (int i=0;i<numReverseChaikinPasses;i++) {
		filteredLine=line->reverseChaikin();
		delete line;
		line=filteredLine;
	}

	CatmullRomSpline *spline=new CatmullRomSpline(line);

	spline->drawWithObject(obj, maxstrokewidth);

	delete spline;
	delete line;

}
