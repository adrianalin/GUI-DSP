#include <qstatusbar.h>
#include <qlabel.h>
#include <qlayout.h>
#include <qevent.h>
#include <qdatetime.h>
#include <qwt_plot_canvas.h>
#include "leftpanel.h"
#include "plot.h"
#include "mainwindow.h"
#include "includes.h"
#include "rightpannel.h"

/*
 **Adaug cele 2 panouri (cu taburile aferente) pe interfata grafica.
 *
 **Aici fac legaturile intre cele 2 clase principale:
 *d_panel - este obiectul in care se introduce inputul - sunt preluate datele de la utilizator si sunt calculati
 *coeficientii filtrului si polii
 *r_panel - este obiectul in care sunt afisati polii, si coeficientii
 */
MainWindow::MainWindow( QWidget *parent ): QMainWindow( parent )
{
    QWidget *w = new QWidget( this );

    input_pannel = new LeftPanel( w );

    output_pannel = new RightPannel( w );

    QGridLayout *MainLayout = new QGridLayout( w );
    MainLayout->addWidget( input_pannel , 0, 0);
    MainLayout->addWidget( output_pannel, 0, 1);
    setCentralWidget( w );

    connect(input_pannel, SIGNAL(settingsProcessed(const ChebyshevFilterResults&)), output_pannel, SLOT(displayCoefficientsAndPlot(ChebyshevFilterResults)));

    connect(input_pannel, SIGNAL(openedWAVFile(QString)), output_pannel, SLOT(displayWavHeader(QString)));
}

