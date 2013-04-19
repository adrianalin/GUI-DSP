#ifndef PROCESSINGTHREAD_H
#define PROCESSINGTHREAD_H

#include <QThread>
#include <QBool>
#include "wavframes.h"
#include "results.h"

class ProcessingThread : public QObject
{
    Q_OBJECT
public:
    void stopProcessingSlot();
    ProcessingThread(const ChebyshevFilterResults &cr);

public slots:
    void startProcessing();

signals:
    void finished();
    void error(QString err);

private:
    volatile bool stopProcessing;
    ChebyshevFilterResults* chebyResults;
    FILE *WAVFile;

    void play_sample(FILE *file, unsigned int card, unsigned int device, unsigned int channels,
                                  unsigned int rate, unsigned int bits, unsigned int period_size,
                                  unsigned int period_count);
    void chebyFiltering(short int *in_buffer, short int *out_buffer, int vecsize, float sr);
};

#endif // PROCESSINGTHREAD_H
