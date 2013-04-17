#ifndef PROCESSINGTHREAD_H
#define PROCESSINGTHREAD_H

#include <QThread>
#include "wavframes.h"
#include "results.h"

class ProcessingThread : public QObject
{
    Q_OBJECT
public:
    ProcessingThread(const ChebyshevFilterResults &cr);

public slots:
    void startProcessing();

signals:
    void finished();
    void error(QString err);

private:
    ChebyshevFilterResults* chebyResults;
    FILE *WAVFile;
};

#endif // PROCESSINGTHREAD_H
