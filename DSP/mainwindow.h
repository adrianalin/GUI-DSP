#ifndef _MAIN_WINDOW_H_
#define _MAIN_WINDOW_H_

#include <qmainwindow.h>
#include "includes.h"
#include "rightpannel.h"

class Plot;
class Panel;
class QLabel;

class MainWindow: public QMainWindow
{
    Q_OBJECT

public:
    MainWindow( QWidget *parent = NULL );

private:
    LeftPanel *input_pannel;
    RightPannel *output_pannel;
};

#endif
