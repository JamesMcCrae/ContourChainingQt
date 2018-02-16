#ifndef CONTOURCHAIN_H
#define CONTOURCHAIN_H

#include <QtOpenGL>

#include "CatmullRomSpline.h"

#define CHAIN_SEGMENT_CLOSENESS_THRESHOLD 0.0001

using namespace std;

class ContourChain {

	public:

	ContourChain();
    bool connectsToChain(const QVector3D & v1, const QVector3D & v2);
    void addSegmentToChain(const QVector3D & v1, const QVector3D & v2);
	int getLength();
	bool isChainClosed();
	void draw();
	void drawAsSpline(int numReverseChaikinPasses);
	void drawAsSplineOBJ(OBJObject *obj, float maxstrokewidth, int numReverseChaikinPasses);
	void reset();
	void forceChainClosed();

	private:

    bool isCloseEnough(const QVector3D & v1, const QVector3D & v2, float scale);

	bool forceClosed;
    QList <QVector3D> contourChain;

};

#endif
