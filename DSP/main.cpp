#include "mainwindow.h"
#include <qapplication.h>

int main( int argc, char **argv )
{
    qDebug()<<"my pid = "<<getpid();

    QApplication a( argc, argv );

    MainWindow mainWindow;
    mainWindow.resize(900, 600 );
    mainWindow.show();

    return a.exec();
}
