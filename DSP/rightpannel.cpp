#include "rightpannel.h"
#include "QWidget"
#include "plot.h"
#include "QGridLayout"
#include "includes.h"
#include "wavframes.h"
#include <QMessageBox>

RightPannel::RightPannel(QWidget* parent):QTabWidget(parent)
{
    timerUpdateCPULoad = new QTimer;

    connect(timerUpdateCPULoad, SIGNAL(timeout()), this, SLOT(getCPULoad()));
    timerUpdateCPULoad->start(800);

    setTabPosition(QTabWidget::East);
    addTab(createPolesTab(this), "Frecventa");
    addTab(createCoefficientsTab(this), "Coeficienti");
    addTab( createFileDetailsTab(this), "Detalii .wav");
    addTab(createProcTab(this), "Proc");
}

QWidget* RightPannel::createPolesTab(QWidget *parent)
{
    QWidget *page = new QWidget( parent );
    QGridLayout *MainLayout = new QGridLayout(page);
    idealPlot = new Plot;
    MainLayout->addWidget(idealPlot, 0, 0);

    page->setLayout(MainLayout);

    return page;
}

QWidget* RightPannel::createCoefficientsTab(QWidget *parent)
{
    QWidget *page = new QWidget( parent );
    QGridLayout *MainLayout = new QGridLayout();
    TextEditCoeficientiA = new QTextEdit;
    TextEditCoeficientiB = new QTextEdit;

    TextEditCoeficientiA->setReadOnly(true);
    TextEditCoeficientiB->setReadOnly(true);

    MainLayout->addWidget(new QLabel("y[n] = a[0]*x[n] + a1*x[n-1] + a2*x[n-2] + ... + b1*y[n-1] + b2*y[n-2] + b3*y[n-3] + ..."), 0, 1);
    MainLayout->addWidget(new QLabel("Coeficienti a:"), 1, 0);
    MainLayout->addWidget(TextEditCoeficientiA, 1, 1);
    MainLayout->addWidget(new QLabel("Coeficienti b:"), 2, 0);
    MainLayout->addWidget(TextEditCoeficientiB, 2, 1);

    page->setLayout(MainLayout);

    return page;
}

QWidget* RightPannel::createFileDetailsTab(QWidget *parent)
{
    QWidget *page = new QWidget( parent );
    TextEditRiffChunk = new QTextEdit;
    TextEditRiffChunk->setReadOnly(true);
    TextEditFmtChunk = new QTextEdit;
    TextEditFmtChunk->setReadOnly(true);
    TextEditDataChunk = new QTextEdit;
    TextEditDataChunk->setReadOnly(true);

    QGridLayout *MainLayout = new QGridLayout;

    MainLayout->addWidget(new QLabel("RIFF chunk"), 0, 0);
    MainLayout->addWidget(TextEditRiffChunk, 0, 1);
    MainLayout->addWidget(new QLabel("FMT subchunk"), 1, 0);
    MainLayout->addWidget(TextEditFmtChunk, 1, 1);
    MainLayout->addWidget(new QLabel("DATA subchunk"), 2, 0);
    MainLayout->addWidget(TextEditDataChunk, 2, 1);

    page->setLayout(MainLayout);

    return page;
}

QWidget* RightPannel::createProcTab(QWidget *parent)
{
    progressCPUUsage = new QProgressBar;
    progressCPUUsage->setMinimum(0);
    progressCPUUsage->setMaximum(100);
    progressCPUUsage->setOrientation(Qt::Vertical);

    TimeSpentComputingCoeff = new QLineEdit("0");

    QWidget *page = new QWidget( parent );
    QGridLayout *MainLayout = new QGridLayout;
    QSpacerItem *spacer1 = new QSpacerItem(0, 0, QSizePolicy::Minimum, QSizePolicy::Expanding);

    MainLayout->addWidget(new QLabel("Timpul de calcul al coeficientilor [s]:"), 0 ,0);
    MainLayout->addWidget(TimeSpentComputingCoeff, 0, 1);

    MainLayout->addItem(spacer1, 1,0);

    MainLayout->addWidget(new QLabel("CPU"), 3, 0);
    MainLayout->addWidget(progressCPUUsage, 2, 0);

    page->setLayout(MainLayout);

    return page;
}

void RightPannel::plotIdealFilter(const double *rasp)
{
    idealPlot->plotIdealFilter(rasp);
}

void RightPannel::plotRealFilter(const double *rasp)
{
    idealPlot->plotRealFilter(rasp);
}

void RightPannel::displayCoefficients(const double& time_spent ,const double *a, const double *b, const int &np)
{
    QString str;
    for(int i=0; i<np; i++)
        str = str.append(QString("a[%1] = %2\n").arg(i).arg(a[i]));
    TextEditCoeficientiA->setText(str);
    str.clear();

    for(int i=1; i<np; i++)
        str = str.append(QString("b[%1] = %2\n").arg(i).arg(b[i]));
    TextEditCoeficientiB->setText(str);
    str.clear();

    TimeSpentComputingCoeff->setText(QString::number(time_spent));
}

void RightPannel::displayWavHeader(const QString &filePath)
{
    FILE* WAVFile;
    struct riff_wave_header riff_wave_header;
    struct chunk_header chunk_header;
    struct chunk_fmt chunk_fmt;
    int more_chunks = 1;
    QString str;

    WAVFile = fopen(filePath.toAscii(), "rb");
    if(!WAVFile)
    {
        QMessageBox::information(this, tr("DSP"), tr("Nu se poate accesa fisierul!"));
        return ;
    }

    //+++++++++++++++++++identific RIFF chunk++++++++++++++++++++++++++++++++++++++++//
    fread(&riff_wave_header, sizeof(riff_wave_header), 1, WAVFile);
    if ((riff_wave_header.riff_id != ID_RIFF) || (riff_wave_header.wave_id != ID_WAVE))
    {
        QMessageBox::information(this, tr("DSP"), tr("Fisierul nu este RIFF/WAV!"));
        fclose(WAVFile);
    }

    str = str.append("Chunk ID: RIFF\n");
    str = str.append(QString("ChunkSize: %1\n").arg(riff_wave_header.riff_sz));
    str = str.append("WaveID: WAVE\n");
    TextEditRiffChunk->setText(str);
    str.clear();

    do {
        fread(&chunk_header, sizeof(chunk_header), 1, WAVFile);

        switch (chunk_header.id) {
        case ID_FMT:
            fread(&chunk_fmt, sizeof(chunk_fmt), 1, WAVFile);
            /* If the format header is larger, skip the rest */
            if (chunk_header.sz > sizeof(chunk_fmt))
                fseek(WAVFile, chunk_header.sz - sizeof(chunk_fmt), SEEK_CUR);

            str = str.append("Subchunk1ID: fmt \n");
            str = str.append(QString("Subchunk1 Size: %1\n").arg(chunk_header.sz));
            str = str.append(QString("Audio format: %1\n").arg(chunk_fmt.audio_format));//PCM=1 (i.e. Linear quantization)
            str = str.append(QString("NumChannels: %1\n").arg(chunk_fmt.num_channels));
            str = str.append(QString("SampleRate: %1\n").arg(chunk_fmt.sample_rate));
            str = str.append(QString("ByteRate: %1\n").arg(chunk_fmt.byte_rate));//=SampleRate*NumChannels*BitsPerSample/8
            str = str.append(QString("BlockAlign: %1\n").arg(chunk_fmt.block_align));//=NumChannels*BitsPerSample/8
            str = str.append(QString("BitsPerSample: %1\n").arg(chunk_fmt.bits_per_sample));
            TextEditFmtChunk->setText(str);
            str.clear();
            break;
        case ID_DATA:
            /* Stop looking for chunks */
            str = str.append("Subchunk2ID: data \n");
            str = str.append(QString("Subchunk2 Size: %1\n").arg(chunk_header.sz));
            str = str.append("data...");
            TextEditDataChunk->setText(str);
            str.clear();
            more_chunks = 0;
            break;
        default:
            /* Unknown chunk, skip bytes */
            fseek(WAVFile, chunk_header.sz, SEEK_CUR);
        }
    } while (more_chunks);
    fclose(WAVFile);
}

//http://stackoverflow.com/questions/3017162/how-to-get-total-cpu-usage-in-linux-c
void RightPannel::getCPULoad()
{
    long int val;
    static int indicator = 0;
    static long int totalJiffies1 = 0;  //suma tuturor valorilor (primele 7) la momentul 1
    static long int workJiffies1 = 0;   //suma valorilor worker (primele 3- user, nice, system) la momentul 1
    static long int totalJiffies2 = 0;  //suma tuturor valorilor (primele 7) la momentul 2
    static long int workJiffies2 = 0;   //suma valorilor worker (primele 3)- user, nice, system) la momentul 2
    long int workOverPeriod;
    long int totalOverPeriod;

    FILE *pFile;
    char s[4];
    pFile = fopen("/proc/stat","r");

    if(!pFile)
    {
        QMessageBox::warning(this, tr("DSP"), tr("Nu se poate accesa /proc/stat !"));
        return ;
    }

    fscanf(pFile, "%s", s); //CPU string

    if(indicator == 0)
    {
        for(int i=0; i<7; i++) //suma primelor 7 valori //momentul t1
        {
            fscanf(pFile, "%d", &val);
            totalJiffies1 +=val;
            if(i<3) //suma primelor 3 valori
                workJiffies1 += val;
        }
        indicator = 1;
    }
    else
    {
        for(int i=0; i<7; i++) //suma primelor 7 valori //momentul t2
        {
            fscanf(pFile, "%d", &val);
            totalJiffies2 +=val;
            if(i<3) //suma primelor 3 valori
                workJiffies2 += val;
        }

        workOverPeriod = workJiffies2 - workJiffies1;
        totalOverPeriod = totalJiffies2 - totalJiffies1;

        progressCPUUsage->setValue((int)((workOverPeriod*100)/totalOverPeriod));

        totalJiffies1 = 0;
        totalJiffies2 = 0;
        workJiffies1 = 0;
        workJiffies2 = 0;
        indicator = 0;
    }

    fclose(pFile);
}
