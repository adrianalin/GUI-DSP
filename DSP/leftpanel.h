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
#include "processingthread.h"

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
    void openedWAVFile(const QString & );

private Q_SLOTS:
    void editedChebyshevParameters();
    void startProcessingChebyshevParameters();
    void OpenFileSlot();
    void startProcessingWAVFileSlot();
    void stopProcessingWAVFileSlot();

private:
    QWidget *createChebyshevTab( QWidget * );
    QWidget *createOpenFileTab( QWidget * );
    QWidget *createCurveTab( QWidget * );

    ChebyshevFilterResults *filteringResults;
    QPushButton *ButtonOpen;
    QPushButton *ButtonStartProcessing;
    QPushButton *ButtonStopProcessing;
    QLineEdit *LineEditFilePath;
    ProcessingThread *worker;

    QwtSlider *sliderCutoffFreq;
    QwtSlider *sliderRipple;
    QwtSlider *sliderNumberOfPoles;

    QLineEdit *LineEditCutoffFreq;
    QLineEdit *LineEditRipple;
    QLineEdit *LinEditNumberOfPoles;
    QComboBox *ComboFilterType;
};

#endif
