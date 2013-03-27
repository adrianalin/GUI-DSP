#include <qstatusbar.h>
#include <qlabel.h>
#include <qlayout.h>
#include <qevent.h>
#include <qdatetime.h>
#include <qwt_plot_canvas.h>
#include "panel.h"
#include "plot.h"
#include "mainwindow.h"
#include "includes.h"

MainWindow::MainWindow( QWidget *parent ): QMainWindow( parent )
{
    QWidget *w = new QWidget( this );

    d_panel = new Panel( w );

    d_plot = new Plot( w );

    QHBoxLayout *hLayout = new QHBoxLayout( w );
    hLayout->addWidget( d_panel );
    hLayout->addWidget( d_plot, 10 );

    setCentralWidget( w );

    connect(d_panel, SIGNAL(settingsProcessed(ChebyshevFilterResults&)), this, SLOT(applySettings(ChebyshevFilterResults&)));
}

void MainWindow::applySettings( ChebyshevFilterResults& settings )
{
    d_plot->applySettings( settings );
}
