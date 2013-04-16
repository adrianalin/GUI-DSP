#include "rightpannel.h"
#include "QWidget"
#include "plot.h"
#include "QGridLayout"
#include "includes.h"
#include "wavframes.h"
#include <QMessageBox>
#include <QApplication>

RightPannel::RightPannel(QWidget* parent):QTabWidget(parent)
{
    setTabPosition(QTabWidget::East);
    addTab(createPolesTab(this), "Poli");
    addTab(createCoefficientsTab(this), "Coeficienti");
    addTab( createFileDetailsTab(this), "Detalii .wav");

    chebyResults = new ChebyshevFilterResults;
}

QWidget* RightPannel::createPolesTab(QWidget *parent)
{
    QWidget *page = new QWidget( parent );
    QGridLayout *MainLayout = new QGridLayout(page);
    d_plot = new Plot;
    MainLayout->addWidget(d_plot, 0, 0);

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

    MainLayout->addWidget(new QLabel("Coeficienti a:"), 0, 0);
    MainLayout->addWidget(TextEditCoeficientiA, 0, 1);
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

void RightPannel::applyResults(const ChebyshevFilterResults &results)
{
    chebyResults->a = results.a;
    chebyResults->b = results.b;
    chebyResults->number_of_poles = results.number_of_poles;
    chebyResults->pole = results.pole;

    displayChebyFilterCoefficients();

    d_plot->applySettings(results); //afisez polii in planul z
}

void RightPannel::displayChebyFilterCoefficients()
{
    QString str;

    for(int i=0;i<22;i++)
    {
        if(chebyResults->a[i]!=0)
            str=str.append(QString("a%1=%2\n").arg(i).arg(chebyResults->a[i]));
    }

    TextEditCoeficientiA->setText(str);
    str.clear();

    for(int i=0;i<22;i++)
    {
        if(chebyResults->b[i]!=0)
            str=str.append(QString("b%1=%2\n").arg(i).arg(chebyResults->b[i]));
    }

    TextEditCoeficientiB->setText(str);
}

void RightPannel::displayWavHeader(const QString &filePath)
{
    struct riff_wave_header riff_wave_header;
    struct chunk_header chunk_header;
    struct chunk_fmt chunk_fmt;
    int more_chunks = 1;
    QString str;

    chebyResults->FilePath = filePath;

    WAVFile = fopen(chebyResults->FilePath.toAscii(), "rb");
    if(!WAVFile)
        QMessageBox::information(this, tr("DSP"), tr("Nu se poate accesa fisierul!"));

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

void RightPannel::startProcessing()
{
    struct riff_wave_header riff_wave_header;
    struct chunk_header chunk_header;
    struct chunk_fmt chunk_fmt;
    unsigned int device = 0;
    unsigned int card = 0;
    unsigned int period_size = 1024;
    unsigned int period_count = 4;
    int more_chunks = 1;

    if(chebyResults->FilePath.isNull())
    {
        QMessageBox::warning(this, tr("DSP"), tr("Introduceti un fiser!"));
        return ;
    }

    WAVFile = fopen(chebyResults->FilePath.toAscii(), "rb");
    if (!WAVFile)
    {
        QMessageBox::warning(this, tr("DSP"), tr("Nu se poate accesa fisierul!"));
        return ;
    }

    fread(&riff_wave_header, sizeof(riff_wave_header), 1, WAVFile);
    if ((riff_wave_header.riff_id != ID_RIFF) ||(riff_wave_header.wave_id != ID_WAVE))
    {
        QMessageBox::warning(this, tr("DSP"), tr("Nu se poate accesa fisierul!"));
        fclose(WAVFile);
        return ;
    }

    do {
        fread(&chunk_header, sizeof(chunk_header), 1, WAVFile);

        switch (chunk_header.id) {
        case ID_FMT:
            fread(&chunk_fmt, sizeof(chunk_fmt), 1, WAVFile);
            /* If the format header is larger, skip the rest */
            if (chunk_header.sz > sizeof(chunk_fmt))
                fseek(WAVFile, chunk_header.sz - sizeof(chunk_fmt), SEEK_CUR);
            break;
        case ID_DATA:
            /* Stop looking for chunks */
            more_chunks = 0;
            break;
        default:
            /* Unknown chunk, skip bytes */
            fseek(WAVFile, chunk_header.sz, SEEK_CUR);
        }
    } while (more_chunks);

    play_sample(WAVFile, card, device, chunk_fmt.num_channels, chunk_fmt.sample_rate,
                chunk_fmt.bits_per_sample, period_size, period_count);

    fclose(WAVFile);
}

static int cclose = 0;

void stream_close(int sig)
{
    /* allow the stream to be closed gracefully */
    signal(sig, SIG_IGN);
    cclose = 1;
}

void RightPannel::play_sample(FILE *file, unsigned int card, unsigned int device, unsigned int channels,
                              unsigned int rate, unsigned int bits, unsigned int period_size,
                              unsigned int period_count)
{
    struct pcm_config config;
    struct pcm *pcm;
    char *buffer;
    int size;
    int num_read;

    config.channels = channels;
    config.rate = rate;
    config.period_size = period_size;
    config.period_count = period_count;
    if (bits == 32)
        config.format = PCM_FORMAT_S32_LE;
    else if (bits == 16)
        config.format = PCM_FORMAT_S16_LE;
    config.start_threshold = 0;
    config.stop_threshold = 0;
    config.silence_threshold = 0;

    /*if (!sample_is_playable(card, device, channels, rate, bits, period_size, period_count)) {
        return;
    }*/

    pcm = pcm_open(card, device, PCM_OUT, &config);
    if (!pcm || !pcm_is_ready(pcm))
    {
        QMessageBox::warning(this, tr("DSP"), tr("Nu se poate accesa dispozitivul!"));
        return;
    }

    size = pcm_frames_to_bytes(pcm, pcm_get_buffer_size(pcm));
    qDebug()<<"\npcm_buffer_siz[frames]=%d\n"<<pcm_get_buffer_size(pcm);
    qDebug()<<"buffer size [bytes]=%d\n"<<size;
    buffer = (char*)malloc(size);
    if (!buffer)
    {
        QMessageBox::warning(this, tr("DSP"), tr("Nu se poate aloca memorie pentru buffer!"));
        free(buffer);
        pcm_close(pcm);
        return;
    }

    qDebug()<<"Playing sample:"<<channels<<" ch,"<<rate<<" hz,"<<bits<<" bits";

    /* catch ctrl-c to shutdown cleanly */
    signal(SIGINT, stream_close);

    do
    {
        num_read = fread(buffer, 1, size, file);
        //MyDelays::msleep(500);
        if (num_read > 0)
        {
            if (pcm_write(pcm, buffer, num_read))
            {
                fprintf(stderr, "Error playing sample\n");
                break;
            }
        }
        qApp->processEvents();
    } while (!cclose && num_read > 0);

    free(buffer);
    pcm_close(pcm);
}

