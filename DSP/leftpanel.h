#ifndef _PANEL_H_
#define _PANEL_H_ 1

#include "settings.h"
#include <qtabwidget.h>
#include <QLineEdit>
#include <QGroupBox>
#include "chebyshev.h"
#include "qwt_knob.h"
#include <QLabel>
#include "includes.h"
#include "plot.h"
#include "qwt_slider.h"

class QComboBox;
class SpinBox;
class CheckBox;

class LeftPanel: public QTabWidget
{
    Q_OBJECT

public:
    LeftPanel( QWidget * = NULL );

Q_SIGNALS:
    void settingsProcessed(const ChebyshevFilterResults & );
    void startProcessingWAVFile();
    void openedWAVFile(const QString & );

private Q_SLOTS:
    void editedChebyshevParameters();
    void startProcessingChebyshevParameters();
    void OpenFileSlot();
    void startProcessingSlot();

private:
    QWidget *createChebyshevTab( QWidget * );
    QWidget *createOpenFileTab( QWidget * );
    QWidget *createCurveTab( QWidget * );
    ChebySettings *chebyInputSettings;
    ChebyshevFilterResults *chebyResults;
    QPushButton *ButtonOpen;
    QPushButton *ButtonStartProcessing;
    QLineEdit *LineEditFilePath;

    struct ChebyshevFilterControls
    {
        QwtSlider *sliderCutoffFreq;
        QwtSlider *sliderRipple;
        QwtSlider *sliderNumberOfPoles;

        QLineEdit *d_cutoffFreq;
        QLineEdit *d_ripple;
        QLineEdit *d_numberOfPoles;
        QLabel *a_chebyshev_coefficients;
        QLabel *b_chebyshev_coefficients;
        QComboBox *d_filterType;
    }chebyControls;
};

#endif
