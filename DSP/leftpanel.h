#ifndef _PANEL_H_
#define _PANEL_H_ 1

#include <qtabwidget.h>
#include <QLineEdit>
#include <QGroupBox>
#include <QLabel>
#include "includes.h"
#include "plot.h"
#include "qwt_slider.h"
#include "processingthread.h"
#include <QProgressBar>

class QComboBox;
class SpinBox;
class CheckBox;

class LeftPanel: public QTabWidget
{
    Q_OBJECT

public:
    LeftPanel( QWidget * = NULL );

Q_SIGNALS:
    void plotIdealFilter(const double *rasp);
    void plotRealFilter(const double *rasp);
    void openedWAVFile(const QString & );
    void showTimeSpentAndCoefficients(const double &time_spent ,const double* a, const double* b, const int& np);

private Q_SLOTS:
    void editedParameters();
    void calculateCoefficients();
    void OpenFileSlot();
    void startProcessingWAVFileSlot();
    void stopProcessingWAVFileSlot();
    void enableStartDisableStopButton();

private:
    QWidget *createFilterTab( QWidget * );
    QWidget *createOpenFileTab( QWidget * );
    QWidget *createCurveTab( QWidget * );

    double *acoef;
    double *bcoef;

    double *idealFreq;

    QPushButton *ButtonOpen;
    QPushButton *ButtonCalculateCoefficients;
    QPushButton *ButtonStartProcessing;
    QPushButton *ButtonStopProcessing;
    QLineEdit *LineEditFilePath;

    QProgressBar* progressBar;

    ProcessingThread *worker;

    QwtSlider *sliderCutoffFreq;
    QwtSlider *sliderNumberOfPoles;
    QwtSlider *sliderStage;

    QLineEdit *LineEditCutoffFreq;
    QLineEdit *LinEditNumberOfPoles;
    QLineEdit *LineEditStage;
    QComboBox *ComboFilterType;
};

#endif
