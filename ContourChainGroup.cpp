#include "ContourChainGroup.h"

ContourChainGroup::ContourChainGroup() {
}

void ContourChainGroup::addSegmentToGroup(const QVector3D &v1, const QVector3D &v2)
{
	waitingForAdd.push_front(v1);
	waitingForAdd.push_front(v2);

	while (tryAddingWaitingToGroup()) { };
}

void ContourChainGroup::finishedAdding() {

	//there are no more pairs coming... so chains that aren't yet closed are never going to be, so force-close them
	while (waitingForAdd.size()>0) {

        for (ContourChain & c : contourChainGroup) {
            if (!c.isChainClosed()) {
                c.forceChainClosed();
            }
		}

		while (tryAddingWaitingToGroup()) { };
	}

}

bool ContourChainGroup::tryAddingWaitingToGroup()
{

    //REFACTORTODO - do not make this a while loop

	//go through each chain and try to add, if we can't return false
	//if we can, return true

	//chains are "done" when they are closed
	bool haveOpenChains=false;

    for (ContourChain & c : contourChainGroup) {
        if (!c.isChainClosed()) {
			//printf("Already have a chain that's open.\n",contourChainGroup.size());
			haveOpenChains=true;
			break;
		}

	}

	if (!haveOpenChains) {		
        contourChainGroup.push_front(ContourChain());
		//printf("\tI made a new chain...  There were no open ones...\n");
	}

    //we iterate through each pair in the waiting list, and try to add them to each chain in the group that is not yet closed
	//printf("%i segments waiting to get in, on %i lines.\n",waitingForAdd.size()/2,contourChainGroup.size());
    for (int i=0; i+1<waitingForAdd.size(); i+=2) {

        QVector3D & v1= waitingForAdd[i];
        QVector3D & v2= waitingForAdd[i+1];

        for (ContourChain & c : contourChainGroup) {
            if (c.isChainClosed()) {
                continue;
            }

            if (c.connectsToChain(v1,v2)) {

				//printf("\tAdding segment to chain!\n");
				//printf("\tLength before: %i\n",eachChain.getLength());
                c.addSegmentToChain(v1,v2);
				//printf("\tLength after: %i\n",eachChain.getLength());
                waitingForAdd.removeAt(i);
                waitingForAdd.removeAt(i);

				return true;
			}
		}
	}

	return false;

}

void ContourChainGroup::draw(int minChainLength)
{
    for (ContourChain & c : contourChainGroup) {
        if (c.getLength()>=minChainLength) {
            c.draw();
        }
	}
}

void ContourChainGroup::drawAsSplines(int minChainLength, float maxstrokewidth, int numReverseChaikinPasses) {
	glLineWidth(maxstrokewidth);
    for (ContourChain & c : contourChainGroup) {
        if (c.getLength()>=minChainLength) {
            c.drawAsSpline(numReverseChaikinPasses);
        }
	}
}

void ContourChainGroup::drawAsSplineOBJ(int minChainLength, OBJObject *obj, float maxstrokewidth, int numReverseChaikinPasses) {
    for (ContourChain & c : contourChainGroup) {
        if (c.getLength()>=minChainLength) {
            c.drawAsSplineOBJ(obj, maxstrokewidth, numReverseChaikinPasses);
        }
    }
}

void ContourChainGroup::resetGroup() {
    for (ContourChain & c : contourChainGroup) {
        c.reset();
	}
	contourChainGroup.clear();
	waitingForAdd.clear();
}
