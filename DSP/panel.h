#ifndef _PANEL_H_
#define _PANEL_H_ 1

#include "settings.h"
#include <qtabwidget.h>
#include <QLineEdit>
#include<QGroupBox>
#include"chebyshev.h"
#include <QLabel>
#include "includes.h"

class QComboBox;
class SpinBox;
class CheckBox;

class Panel: public QTabWidget
{
    Q_OBJECT

public:
    Panel( QWidget * = NULL );

Q_SIGNALS:
    void settingsProcessed( ChebyshevFilterResults & );

private Q_SLOTS:
    void editedChebyshevParameters();

private:
    QWidget *createChebyshevTab( QWidget * );
    QWidget *createCanvasTab( QWidget * );
    QWidget *createCurveTab( QWidget * );

    struct ChebyshevFilterControls
    {
        QLineEdit *d_cutoffFreq;
        QLineEdit *d_ripple;
        QLineEdit *d_numberOfPoles;
        QLabel *a_chebyshev_coefficients;
        QLabel *b_chebyshev_coefficients;
        QComboBox *d_filterType;
    }chebyControls;

    void displayChebyFilterCoefficients(double *a, double *b);
    void processChebyshevParameters(ChebySettings s);
};

#endif
