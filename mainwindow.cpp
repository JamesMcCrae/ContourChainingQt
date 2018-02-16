#include "mainwindow.h"

MainWindow::MainWindow()
{
    glwidget = new GLWidget();

    setWindowTitle("Contour Chaining for Stroke Stylization" );

    //set screen size
    QRect screenSize = QApplication::desktop()->screenGeometry();
    setGeometry(100, 100, screenSize.width()-200, screenSize.height()-200);

    splitter = new QSplitter();
    splitter->addWidget(glwidget);
    splitter->addWidget(glwidget->w);

    setCentralWidget(splitter);

//    glutDisplayFunc( myGlutDisplay );
//    GLUI_Master.set_glutReshapeFunc( myGlutReshape );
//    GLUI_Master.set_glutKeyboardFunc( myGlutKeyboard );
//    GLUI_Master.set_glutSpecialFunc( NULL );
//    GLUI_Master.set_glutMouseFunc( myGlutMouse );
//    glutMotionFunc( myGlutMotion );
//    glutPassiveMotionFunc( myGlutPassiveMotion);

//    ilInit();
//    iluInit();
//    ilutRenderer(ILUT_OPENGL);

    /*** Create the side subwindow ***/
//    glui = GLUI_Master.create_glui_subwindow( main_window,
//                                              GLUI_SUBWINDOW_RIGHT );

//    glutInitDisplayMode( GLUT_RGB | GLUT_DOUBLE);
//    glutInitWindowPosition( 50, 50 );
//    glutInitWindowSize( 1000, 800 );



    /**** Link windows to GLUI, and register idle callback ******/
//    glui->set_main_gfx_window( main_window );

    /**** We register the idle callback with GLUI, *not* with GLUT ****/
//    GLUI_Master.set_glutIdleFunc( myGlutIdle );

    CreateMenus();
}

void MainWindow::CreateMenus()
{
    openObjAct = new QAction(tr("&Open OBJ"), this);
    openObjAct->setShortcuts(QKeySequence::Open);
    openObjAct->setStatusTip(tr("Open an OBJ file"));
    connect(openObjAct, &QAction::triggered, this, &MainWindow::openObj);

    openBrushObjAct = new QAction(tr("&Open Brush OBJ"), this);
    openBrushObjAct->setShortcuts(QKeySequence::Open);
    openBrushObjAct->setStatusTip(tr("Open an OBJ file to be used as brush geometry"));
    connect(openBrushObjAct, &QAction::triggered, this, &MainWindow::openBrushObj);

    exitAct = new QAction(tr("E&xit"), this);
    exitAct->setShortcuts(QKeySequence::Quit);
    exitAct->setStatusTip(tr("Open an OBJ file"));
    connect(exitAct, &QAction::triggered, this, &MainWindow::exit);

    fileMenu = menuBar()->addMenu(tr("&File"));
    fileMenu->addAction(openObjAct);
    fileMenu->addAction(openBrushObjAct);
    fileMenu->addSeparator();
    fileMenu->addAction(exitAct);
}

void MainWindow::openObj()
{
    QString fileName = QFileDialog::getOpenFileName(this, tr("Open OBJ file"), "", tr("Wavefront OBJ (*.obj);;All Files (*)"));
    if (!fileName.isNull()) {
        glwidget->LoadOBJ(fileName);
    }
}

void MainWindow::openBrushObj()
{
    QString fileName = QFileDialog::getOpenFileName(this, tr("Open Brush OBJ file"), "", tr("Wavefront OBJ (*.obj);;All Files (*)"));
    if (!fileName.isNull()) {
        glwidget->LoadBrushOBJ(fileName);
    }
}

void MainWindow::exit()
{
    QApplication::quit();
}
