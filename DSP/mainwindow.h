#ifndef _MAIN_WINDOW_H_
#define _MAIN_WINDOW_H_

#include "settings.h"
#include <qmainwindow.h>
#include"includes.h"

class Plot;
class Panel;
class QLabel;
class Settings;

class MainWindow: public QMainWindow
{
    Q_OBJECT

public:
    MainWindow( QWidget *parent = NULL );

private Q_SLOTS:
    void applySettings(ChebyshevFilterResults &settings );

private:
    Plot *d_plot;
    Panel *d_panel;
};

#endif
