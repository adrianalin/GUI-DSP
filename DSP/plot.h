#ifndef _PLOT_H_
#define _PLOT_H_ 1

#include <qwt_plot.h>
#include <qwt_system_clock.h>
#include <qwt_symbol.h>
#include <qwt_plot_marker.h>
#include "settings.h"
#include "chebyshev.h"

class QwtPlotGrid;
class QwtPlotCurve;

class Plot: public QwtPlot
{
    Q_OBJECT

public:
    Plot( QWidget* = NULL );

public Q_SLOTS:
    void applySettings(const ChebyshevFilterResults &chebyResults);

private:
    void alignScales();
    QwtPlotGrid *d_grid;
    QwtPlotCurve *d_curve;
};

#endif
