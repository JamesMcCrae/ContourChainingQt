/*
 * Line.cpp - cpp file for Line class.
 */

#include <stdlib.h>

#include "Line.h"

#define RAD2DEG(x) x*180.0/3.14159

void Line::addComponentAtEnd(QVector3D v) {
    components.push_back(v);
}

int Line::getNumComponents() {
    return components.size();
    
}

QVector3D Line::getComponent(int componentIndex) {
    
    if (componentIndex<0||componentIndex>=components.size()) {
        fprintf(stderr,"Program should halt...  Line passed index %i, contains %i components.\n",
            componentIndex, components.size());
    }
    
    return components[componentIndex];
    
}

QVector3D Line::getDirection(int componentIndex) {
    
    if (componentIndex<0||componentIndex>=components.size()) {

        fprintf(stderr,"Program halting...  Line passed index %i, contains %i components.\n",
            componentIndex, components.size());

    }
    
    if (componentIndex>0)
        return components[componentIndex]-components[componentIndex-1];
    else
        return components[1]-components[0];
    
}

QVector3D Line::getEndComponent() {
    
    if (components.size()==0) {
        fprintf(stderr,"Program halting...  Asked for end component of empty line.\n");
    }
    
    return components[components.size()-1];
    
}

QVector3D Line::getEndDirection() {
    
    if (components.size()<2) {
        fprintf(stderr,"Halting...  Line needs at least 2 components to have direction.\n");   
    }
        
    return components[components.size()-1]-components[components.size()-2];
    
}

QPointer <Line> Line::reverseChaikin()
{
    QPointer <Line> newLine=new Line();

	//return this line if there's not enough points
	if (components.size()<=3) {
		for (int i=0;i<components.size();i++) {
			newLine->addComponentAtEnd(components[i]);
		}
		return newLine;
	}

	//otherwise, return this line after a single pass of reverse chaikin method
	int newNumComponents=ceil((float)components.size()/2.0);
	
	newLine->addComponentAtEnd(components[0]);
	for (int i=1;i<newNumComponents-1;i++) {
		//q_j=-1/4p_i-1+3/4p_i+3/4p_i+1-1/4p_i+2
        QVector3D eachComp=components[i*2-1]*(-0.25)+components[i*2]*(0.75)
						+components[i*2+1]*(0.75)+components[i*2+2]*(-0.25);
		newLine->addComponentAtEnd(eachComp);
	}
	newLine->addComponentAtEnd(components[components.size()-1]);	

	return newLine;
}
