#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QtGui>

#include "glwidget.h"

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow();

private slots:

    void openObj();
    void openBrushObj();
    void exit();

private:

    void CreateMenus();

    QPointer <QSplitter> splitter;
    QPointer <GLWidget> glwidget;

    QMenu *fileMenu;
    QAction *openObjAct;
    QAction *openBrushObjAct;
    QAction *exitAct;

};

#endif // MAINWINDOW_H
