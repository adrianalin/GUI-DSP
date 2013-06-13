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

Plot::Plot( QWidget *parent ): QwtPlot( parent )
{
    for(int i = 0; i<256; i++)
    {
        frequencyFFT[i] = (double)i;
        if(i<128)
            frequency[i] = (double)i;
    }

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
    setAxisScale(QwtPlot::xBottom, 0, 255);
    //setAxisScaleEngine( QwtPlot::xBottom, new QwtLogScaleEngine );

    //curves
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

void Plot::plotIdealFilter(const double* rasp )
{
    d_curve1->setSamples(frequency, rasp, 128);
    d_curve1->plot();
    replot();
}

void Plot::plotRealFilter(const double *rasp)
{
    d_curve2->setSamples(frequencyFFT, rasp, 255);
    d_curve1->plot();
    replot();
}
