#ifndef CONTOURCHAINGROUP_H
#define CONTOURCHAINGROUP_H

#include "ContourChain.h"

class ContourChainGroup {

	public:

	ContourChainGroup();
    void addSegmentToGroup(const QVector3D & v1, const QVector3D & v2);
	void draw(int minChainLength);
	void drawAsSplines(int minChainLength, float maxstrokewidth, int numReverseChaikinPasses);
	void drawAsSplineOBJ(int minChainLength, OBJObject *obj, float maxstrokewidth, int numReverseChaikinPasses);
	void resetGroup();
	void finishedAdding();

	private:

	bool tryAddingWaitingToGroup();

    QList <ContourChain> contourChainGroup;
    QList <QVector3D> waitingForAdd;

};

#endif
