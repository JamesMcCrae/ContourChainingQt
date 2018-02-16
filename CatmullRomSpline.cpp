/*
 * CatmullRomSpline.cpp - body for CatmullRomSpline class
 */

#include "CatmullRomSpline.h"

CatmullRomSpline::CatmullRomSpline(Line *line) {

	lineToFit=line;
	drawSampleRate=DEFAULT_DRAWSAMPLERATE;

	if (lineToFit->getNumComponents()<2) {
		lineToFit=NULL;
		return;
	}

	/*
	 * Calculate list of tangents
	 */

	//First and last tangents will just be vector toward line	
    tangentList=QVector<QVector3D>(lineToFit->getNumComponents());
	
	tangentList[0]=lineToFit->getDirection(0);
	tangentList[lineToFit->getNumComponents()-1]=lineToFit->getEndDirection();	

	float tension=0.0;

	//Next calculate tangents for intermediate points
	for (int i=1;i<lineToFit->getNumComponents()-1;i++) {
		tangentList[i]=(lineToFit->getComponent(i+1)-lineToFit->getComponent(i-1))*0.5*(1.0-tension);
	}	
}

QVector3D CatmullRomSpline::interp(int segment, double interpAmt) {

    QVector3D returnPoint=lineToFit->getComponent(segment)*(2.0*pow(interpAmt,3)-3.0*pow(interpAmt,2)+1.0)+
						tangentList[segment]*(pow(interpAmt,3)-2.0*pow(interpAmt,2)+interpAmt)+
						lineToFit->getComponent(segment+1)*(-2.0*pow(interpAmt,3)+3.0*pow(interpAmt,2))+
						tangentList[segment+1]*(pow(interpAmt,3)-pow(interpAmt,2));

	return returnPoint;
}

float CatmullRomSpline::findYgivenX(int segment, float desiredx) {

	float t_low=0.0;
	float t_high=1.0;
	int numCuts=20;
	
    QVector3D bestVecYet;

	for (int k=0;k<numCuts;k++) {
        bestVecYet = interp(segment,(t_low+t_high)/2.0);
        if (bestVecYet.x() < desiredx) {
			t_low=(t_low+t_high)/2.0;
        }
        else {
			t_high=(t_low+t_high)/2.0;
        }
	}

    return bestVecYet.y();
}

void CatmullRomSpline::drawWithObject(OBJObject *obj, float maxstrokewidth) {

	if (lineToFit==NULL||lineToFit->getNumComponents()<2)
		return;

	Line *zLine=new Line();
	for (int i=0;i<lineToFit->getNumComponents();i++) {
        QVector3D eachVec=lineToFit->getComponent(i);
        eachVec.setY(eachVec.z());
		zLine->addComponentAtEnd(eachVec);
	}

    CatmullRomSpline * zInterpSpline = new CatmullRomSpline(zLine);

	glEnable(GL_TEXTURE_2D);   
	glColor3f(1.0,1.0,1.0);
    glPushMatrix();
	
	//warp object to follow each spline piece
	for (int i=0;i<lineToFit->getNumComponents()-1;i++) { //for i components (points), you have i-1 segments
		
        QVector <QVector3D> verts = obj->getVerts();
        QVector <QVector3D> warpVertexes = QVector<QVector3D>(verts.size());
	
		//warp object vertices to match what's going on here
        for (int j=0;j<verts.size();j++) {
            const QVector3D & v = verts[j];

            QVector3D rightNormal=getRightNormal(i,v.y());

			//we need to find value within widthSpline where x value = i+eachVertex.y, we want the Y value there
            QVector3D warpEachVertex = interp(i,v.y()) + rightNormal*v.x()*maxstrokewidth;
            QVector3D depthVertex=zInterpSpline->interp(i,v.y());

			//x and y values come from spline to fit sketch points
            warpVertexes[j].setX(warpEachVertex.x());
            warpVertexes[j].setY(warpEachVertex.y());
            warpVertexes[j].setZ(depthVertex.y()+v.z()*maxstrokewidth);

		}

        obj->setVerts(warpVertexes);
        obj->draw();

        obj->setVerts(verts);

	}

	glPopMatrix();            
    glDisable(GL_TEXTURE_2D);	

	delete zLine;
	delete zInterpSpline;
}
