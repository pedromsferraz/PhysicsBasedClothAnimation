#include "mainwindow.h"
#include <QApplication>
#include <QSurfaceFormat>

int main(int argc, char *argv[])
{
    //Necessario em MacOS
    QSurfaceFormat format;
    format.setDepthBufferSize(24);
    format.setVersion( 3, 3 ); //versao OpenGL
    format.setProfile(QSurfaceFormat::CoreProfile);
    QSurfaceFormat::setDefaultFormat(format);
    QApplication::setAttribute(Qt::AA_ShareOpenGLContexts);

    QApplication a(argc, argv);

    MainWindow w;
    w.show();

    return a.exec();
}
