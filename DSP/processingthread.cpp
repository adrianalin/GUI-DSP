#include "processingthread.h"
#include <QMessageBox>
#include <stdio.h>
#include <QDebug>

void play_sample(FILE *file, unsigned int card, unsigned int device, unsigned int channels,
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

    //if (!sample_is_playable(card, device, channels, rate, bits, period_size, period_count)) {
    //    return;
    // }

    pcm = pcm_open(card, device, PCM_OUT, &config);
    if (!pcm || !pcm_is_ready(pcm))
    {
        //QMessageBox::warning(this, tr("DSP"), tr("Nu se poate accesa dispozitivul!"));
        return;
    }

    size = pcm_frames_to_bytes(pcm, pcm_get_buffer_size(pcm));
    qDebug()<<"\npcm_buffer_siz[frames]=%d\n"<<pcm_get_buffer_size(pcm);
    qDebug()<<"buffer size [bytes]=%d\n"<<size;
    buffer = (char*)malloc(size);
    if (!buffer)
    {
        //QMessageBox::warning(this, tr("DSP"), tr("Nu se poate aloca memorie pentru buffer!"));
        free(buffer);
        pcm_close(pcm);
        return;
    }

    qDebug()<<"Playing sample:"<<channels<<" ch,"<<rate<<" hz,"<<bits<<" bits";

    // catch ctrl-c to shutdown cleanly

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
    } while (num_read > 0);

    free(buffer);
    pcm_close(pcm);
}

ProcessingThread::ProcessingThread(const ChebyshevFilterResults &cr)
{
    chebyResults = new ChebyshevFilterResults;

    chebyResults->a = cr.a;
    chebyResults->b = cr.b;
    chebyResults->FilePath = cr.FilePath;
    chebyResults->number_of_poles = cr.number_of_poles;
    chebyResults->pole = cr.pole;
}

void ProcessingThread::startProcessing()
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
        //QMessageBox::warning(this, tr("DSP"), tr("Introduceti un fiser!"));
        return ;
    }

    WAVFile = fopen(chebyResults->FilePath.toAscii(), "rb");
    if (!WAVFile)
    {
        //QMessageBox::warning(this, tr("DSP"), tr("Nu se poate accesa fisierul!"));
        return ;
    }

    fread(&riff_wave_header, sizeof(riff_wave_header), 1, WAVFile);
    if ((riff_wave_header.riff_id != ID_RIFF) ||(riff_wave_header.wave_id != ID_WAVE))
    {
        //QMessageBox::warning(this, tr("DSP"), tr("Nu se poate accesa fisierul!"));
        fclose(WAVFile);
        return ;
    }

    do {
        fread(&chunk_header, sizeof(chunk_header), 1, WAVFile);

        switch (chunk_header.id) {
        case ID_FMT:
            fread(&chunk_fmt, sizeof(chunk_fmt), 1, WAVFile);
            // If the format header is larger, skip the rest
            if (chunk_header.sz > sizeof(chunk_fmt))
                fseek(WAVFile, chunk_header.sz - sizeof(chunk_fmt), SEEK_CUR);
            break;
        case ID_DATA:
            //Stop looking for chunks
            more_chunks = 0;
            break;
        default:
            //Unknown chunk, skip bytes
            fseek(WAVFile, chunk_header.sz, SEEK_CUR);
        }
    } while (more_chunks);

    play_sample(WAVFile, card, device, chunk_fmt.num_channels, chunk_fmt.sample_rate, chunk_fmt.bits_per_sample, period_size, period_count);

    fclose(WAVFile);

    emit finished();
}
