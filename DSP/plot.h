#ifndef _PLOT_H_
#define _PLOT_H_ 1

#include <qwt_plot.h>
#include <qwt_system_clock.h>
#include <qwt_symbol.h>

class QwtPlotGrid;
class QwtPlotCurve;

class Plot: public QwtPlot
{
    Q_OBJECT

public:
    Plot( QWidget* = NULL );

public Q_SLOTS:
    void plotIdealFilter(const double* rasp);
    void plotRealFilter(const double* rasp);

private:
    double frequency[128];
    QwtPlotGrid *d_grid;
    QwtPlotCurve *d_curve1;
    QwtPlotCurve *d_curve2;
};

#endif
