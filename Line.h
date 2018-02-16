/*
 * Line.h - header file for the Line class.
 *
 * Author: James McCrae
 */

#ifndef LINE_H
#define LINE_H

#include <QtOpenGL>

class Line : public QObject {
    Q_OBJECT
    
public:

    void addComponentAtEnd(QVector3D v);
    int getNumComponents();
    QVector3D getComponent(int componentIndex);
    QVector3D getDirection(int componentIndex);
    QVector3D getEndComponent();
    QVector3D getEndDirection();
    QPointer <Line> reverseChaikin();

private:
    
    QVector <QVector3D> components;
    
};

#endif
