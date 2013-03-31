#include <qwt_plot_canvas.h>
#include <qwt_plot_grid.h>
#include <qwt_plot_curve.h>
#include <qwt_plot_layout.h>
#include <qwt_scale_widget.h>
#include <qwt_plot_panner.h>
#include <qwt_plot_magnifier.h>
#include "plot.h"
#include "settings.h"

struct gridSettings
{
    QPen pen;
} grid;

Plot::Plot( QWidget *parent ): QwtPlot( parent )
{
    // Assign a title
    setTitle( "Butterworth (Chebyshev) low pass filter s-plane (w = 1)" );

    QwtPlotCanvas *canvas = new QwtPlotCanvas();
    canvas->setFrameStyle( QFrame::Box | QFrame::Plain );
    canvas->setLineWidth( 1 );
    canvas->setPalette( Qt::white );

    setCanvas( canvas );

    // panning with the left mouse button
    ( void ) new QwtPlotPanner( canvas );

    // zoom in/out with the wheel
    ( void ) new QwtPlotMagnifier( canvas );

    alignScales();

    // Insert grid
    d_grid = new QwtPlotGrid();
    grid.pen.setStyle(Qt::DashLine);
    d_grid->setPen(grid.pen);
    d_grid->enableXMin(true);
    d_grid->enableYMin(true);
    d_grid->attach( this );

    // Insert curve
    d_curve = new QwtPlotCurve( "poles and zeroes" );
    d_curve->setSymbol(new QwtSymbol( QwtSymbol::XCross, Qt::NoBrush, QPen( Qt::darkMagenta ), QSize( 8, 8 ) ));
    d_curve->setStyle(QwtPlotCurve::NoCurve);
    d_curve->attach( this );

    //  ...a horizontal line at y = 0...
    QwtPlotMarker *mY = new QwtPlotMarker();
    mY->setLabel( QString::fromLatin1( "y = 0" ) );
    mY->setLabelAlignment( Qt::AlignRight | Qt::AlignTop );
    mY->setLineStyle( QwtPlotMarker::HLine );
    mY->setLinePen( Qt::black, 0, Qt::SolidLine );
    mY->setYValue( 0.0 );
    mY->attach( this );

    //  ...a vertical line at x = 0
    QwtPlotMarker *mX = new QwtPlotMarker();
    mX->setLabel( QString::fromLatin1( "x = 0" ) );
    mX->setLabelAlignment( Qt::AlignLeft | Qt::AlignBottom );
    mX->setLabelOrientation( Qt::Vertical );
    mX->setLineStyle( QwtPlotMarker::VLine );
    mX->setLinePen( Qt::black, 0, Qt::SolidLine );
    mX->setXValue( 0 );
    mX->attach( this );

    // Axis
    setAxisTitle( QwtPlot::xBottom, "re-->" );
    setAxisScale( QwtPlot::xBottom, -5, 5 );

    setAxisTitle( QwtPlot::yLeft, "jw-->" );
    setAxisScale( QwtPlot::yLeft, -5, 5 );
}

//
//  Set a plain canvas frame and align the scales to it
//
void Plot::alignScales()
{
    // The code below shows how to align the scales to
    // the canvas frame, but is also a good example demonstrating
    // why the spreaded API needs polishing.

    for ( int i = 0; i < QwtPlot::axisCnt; i++ )
    {
        QwtScaleWidget *scaleWidget = axisWidget( i );
        if ( scaleWidget )
            scaleWidget->setMargin( 0 );

        QwtScaleDraw *scaleDraw = axisScaleDraw( i );
        if ( scaleDraw )
            scaleDraw->enableComponent( QwtAbstractScaleDraw::Backbone, false );
    }

    plotLayout()->setAlignCanvasToScales( true );
}

void Plot::applySettings(const ChebyshevFilterResults &chebyResults )
{
    double x[chebyResults.number_of_poles], y[chebyResults.number_of_poles];
    int j=0;
    for(int i = 1; i<=chebyResults.number_of_poles/2;i++)
    {
        x[j] = chebyResults.pole[i].rp;
        y[j] = chebyResults.pole[i].ip;
        j++;
        x[j] = chebyResults.pole[i].rp;
        y[j] = -chebyResults.pole[i].ip;
        j++;
    }

    d_curve->setSamples(x, y, chebyResults.number_of_poles);

    replot();
}
