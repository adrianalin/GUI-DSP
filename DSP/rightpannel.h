#ifndef PALYLISTPANNEL_H
#define PALYLISTPANNEL_H
#include "includes.h"


class RightPannel: public QTabWidget
{
    Q_OBJECT

public:
    RightPannel(QWidget * parent = NULL );

public Q_SLOTS:
    void applyResults(const ChebyshevFilterResults &results);
    void startProcessing();
    void displayWavHeader(const QString &filePath);

private:
    ChebyshevFilterResults *chebyResults;

    QTextEdit *TextEditCoeficientiA;
    QTextEdit *TextEditCoeficientiB;
    QTextEdit *TextEditRiffChunk;
    QTextEdit *TextEditFmtChunk;
    QTextEdit *TextEditDataChunk;

    FILE *WAVFile;

    Plot *d_plot;

    QWidget* createPolesTab( QWidget *);
    QWidget* createCoefficientsTab(QWidget *parent);
    QWidget* createFileDetailsTab(QWidget *parent);

    void displayChebyFilterCoefficients();

    void play_sample(FILE *file, unsigned int card, unsigned int device, unsigned int channels,
                     unsigned int rate, unsigned int bits, unsigned int period_size,
                     unsigned int period_count);
};

#endif // PALYLISTPANNEL_H
