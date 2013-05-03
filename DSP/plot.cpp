#include <qwt_plot_canvas.h>
#include <qwt_plot_grid.h>
#include <qwt_plot_curve.h>
#include <qwt_scale_engine.h>
#include <qwt_plot_layout.h>
#include <qwt_scale_widget.h>
#include <qwt_plot_panner.h>
#include <qwt_plot_magnifier.h>
#include <qwt_legend.h>
#include "plot.h"
#include "QGridLayout"

static void logSpace( double *array, int size, double xmin, double xmax )
{
    if ( ( xmin <= 0.0 ) || ( xmax <= 0.0 ) || ( size <= 0 ) )
        return;

    const int imax = size - 1;

    array[0] = xmin;
    array[imax] = xmax;

    const double lxmin = log( xmin );
    const double lxmax = log( xmax );
    const double lstep = ( lxmax - lxmin ) / double( imax );

    for ( int i = 1; i < imax; i++ )
        array[i] = qExp( lxmin + double( i ) * lstep );
}

Plot::Plot( QWidget *parent ): QwtPlot( parent )
{
    for(int i = 0; i<128; i++)
        frequency[i] = (double)i;

    //logSpace(frequency, 128, 0.01, 128.);

    setAutoReplot( false );

    setTitle( "Raspunsul in frecventa" );

    QwtPlotCanvas *canvas = new QwtPlotCanvas();
    canvas->setBorderRadius( 8 );

    setCanvas( canvas );
    setCanvasBackground( QColor( "MidnightBlue" ) );

    // legend
    QwtLegend *legend = new QwtLegend;
    insertLegend( legend, QwtPlot::BottomLegend );

    // grid
    QwtPlotGrid *grid = new QwtPlotGrid;
    grid->enableXMin( true );
    grid->setMajorPen( Qt::white, 0, Qt::DotLine );
    grid->setMinorPen( Qt::gray, 0 , Qt::DotLine );
    grid->attach( this );

    // axes
    setAxisTitle( QwtPlot::xBottom, "Frecventa" );
    setAxisTitle( QwtPlot::yLeft, "Amplitudine [dB]" );
    setAxisScale(QwtPlot::yLeft, 0, 1);

    setAxisMaxMajor( QwtPlot::xBottom, 6 );
    setAxisMaxMinor( QwtPlot::xBottom, 9 );
    setAxisScale(QwtPlot::xBottom, 0, 128);
    //setAxisScaleEngine( QwtPlot::xBottom, new QwtLogScaleEngine );

    // curves
    d_curve1 = new QwtPlotCurve( "Caracteristica ideala" );
    d_curve1->setRenderHint( QwtPlotItem::RenderAntialiased );
    d_curve1->setPen( Qt::green );
    d_curve1->setLegendAttribute( QwtPlotCurve::LegendShowLine );
    d_curve1->setYAxis( QwtPlot::yLeft );
    d_curve1->attach( this );

    d_curve2 = new QwtPlotCurve( "Caracteristica reala" );
    d_curve2->setRenderHint( QwtPlotItem::RenderAntialiased );
    d_curve2->setPen( Qt::cyan );
    d_curve2->setLegendAttribute( QwtPlotCurve::LegendShowLine );
    d_curve2->attach( this );

    setAutoReplot( true );
}

//
//  Set a plain canvas frame and align the scales to it
//
/*void Plot::alignScales()
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
}*/

void Plot::plotIdealFilter(const double* rasp )
{
    d_curve1->setSamples(frequency, rasp, 128);
    d_curve1->plot();
    replot();
}

void Plot::plotRealFilter(const double *rasp)
{
    d_curve2->setSamples(frequency, rasp, 128);
    d_curve1->plot();
    replot();
}
