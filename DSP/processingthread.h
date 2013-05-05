#ifndef PROCESSINGTHREAD_H
#define PROCESSINGTHREAD_H

#include <QThread>
#include <QBool>
#include "wavframes.h"

//#define ZED

class ProcessingThread : public QObject
{
    Q_OBJECT
public:
    void stopProcessingSlot();
    ProcessingThread(double const* coefaa, double const* coefbb, const QString strr, const int& npp, const int& degg);

public slots:
    void startProcessing();

signals:
    void finished();
    void error(QString err);

private:
    bool stopProcessing;
    QString filePath;
    FILE *WAVFile;

#ifdef ZED
    void play_sample(FILE *file_out);
#else
    void play_sample(FILE *file, unsigned int card, unsigned int device, unsigned int channels,
                                  unsigned int rate, unsigned int bits, unsigned int period_size,
                                  unsigned int period_count);
#endif
};

#endif // PROCESSINGTHREAD_H
