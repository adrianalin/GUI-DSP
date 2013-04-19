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
    void displayCoefficientsAndPlot(const ChebyshevFilterResults &results);
    void displayWavHeader(const QString &filePath);

private:
    ChebyshevFilterResults *chebyResults;

    QTextEdit *TextEditCoeficientiA;
    QTextEdit *TextEditCoeficientiB;
    QTextEdit *TextEditRiffChunk;
    QTextEdit *TextEditFmtChunk;
    QTextEdit *TextEditDataChunk;

    QLineEdit *LineEditCPU;

    QTimer* timerUpdateCPULoad;

    Plot *d_plot;

    QWidget* createPolesTab( QWidget *);
    QWidget* createCoefficientsTab(QWidget *parent);
    QWidget* createFileDetailsTab(QWidget *parent);
    QWidget* createProcTab(QWidget *parent);
};

#endif // PALYLISTPANNEL_H
