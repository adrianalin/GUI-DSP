#ifndef PALYLISTPANNEL_H
#define PALYLISTPANNEL_H
#include "includes.h"
#include <QTimer>


class RightPannel: public QTabWidget
{
    Q_OBJECT

public:
    RightPannel(QWidget * parent = NULL );

public Q_SLOTS:
    void getCPULoad();
    void plotIdealFilter(const double *rasp);
    void plotRealFilter(const double *rasp);
    void displayWavHeader(const QString &filePath);
    void displayCoefficients(const double & time_spent ,const double *a, const double *b, const int& np);

private:
    QTextEdit *TextEditCoeficientiA;
    QTextEdit *TextEditCoeficientiB;
    QTextEdit *TextEditRiffChunk;
    QTextEdit *TextEditFmtChunk;
    QTextEdit *TextEditDataChunk;

    QLineEdit *TimeSpentComputingCoeff;

    QProgressBar* progressCPUUsage;

    QTimer* timerUpdateCPULoad;

    Plot *idealPlot;

    QWidget* createPolesTab( QWidget *);
    QWidget* createCoefficientsTab(QWidget *parent);
    QWidget* createFileDetailsTab(QWidget *parent);
    QWidget* createProcTab(QWidget *parent);
};

#endif // PALYLISTPANNEL_H
