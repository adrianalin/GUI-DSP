#include "mainwindow.h"
#include <qapplication.h>

int main( int argc, char **argv )
{
    QApplication a( argc, argv );

    MainWindow mainWindow;
    mainWindow.resize(900, 600 );
    mainWindow.show();

    return a.exec();
}
