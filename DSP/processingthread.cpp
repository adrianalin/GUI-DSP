#include "processingthread.h"
#include <QMessageBox>
#include <stdio.h>
#include <QDebug>

    static int ind = 1;

ProcessingThread::ProcessingThread(const ChebyshevFilterResults &cr)
{
    double *coefa;
    double *coefb;
    complex_number *poli;

    chebyResults = new ChebyshevFilterResults;
    stopProcessing = false;

    for(int i=0; i<22; i++)
    {
        chebyResults->a[i] = cr.a[i];
        chebyResults->b[i] = cr.b[i];
    }
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

    stopProcessing = false;
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
    ind = 0;

    emit finished();
}

void ProcessingThread::play_sample(FILE *file, unsigned int card, unsigned int device, unsigned int channels,
                                   unsigned int rate, unsigned int bits, unsigned int period_size,
                                   unsigned int period_count)
{
    struct pcm_config config;
    struct pcm *pcm;
    short int *in_buffer;
    short int *out_buffer;
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
        qDebug()<<"pcm is not ready!";
        return;
    }

    size = pcm_frames_to_bytes(pcm, pcm_get_buffer_size(pcm)); //size = buffer size (in frames)
    qDebug()<<"\npcm_buffer_size[frames]=%d\n"<<pcm_get_buffer_size(pcm);
    qDebug()<<"buffer size [bytes]=%d\n"<<size;
    in_buffer = (short int*)malloc(size);
    if (!in_buffer)
    {
        //QMessageBox::warning(this, tr("DSP"), tr("Nu se poate aloca memorie pentru buffer!"));
        qDebug()<<"could not load buffer!";
        free(in_buffer);
        pcm_close(pcm);
        return;
    }

    out_buffer = (short int*)malloc(size);
    if (!out_buffer)
    {
        //QMessageBox::warning(this, tr("DSP"), tr("Nu se poate aloca memorie pentru buffer!"));
        qDebug()<<"could not load buffer!";
        free(out_buffer);
        pcm_close(pcm);
        return;
    }

    qDebug()<<"Playing sample:"<<channels<<" ch,"<<rate<<" hz,"<<bits<<" bits";

    //****************************************************************processing loop***********
    do
    {
        num_read = fread(in_buffer, sizeof(short int), size/2, file);
        chebyFiltering(in_buffer, out_buffer, num_read, rate);
        if (num_read > 0)
        {
            if (pcm_write(pcm, out_buffer, num_read*2))
            {
                fprintf(stderr, "Error playing sample\n");
                break;
            }
        }
    } while ((num_read > 0) && (stopProcessing == false));
    //*******************************************************************************************

    free(in_buffer);
    free(out_buffer);
    pcm_close(pcm);
    qDebug()<<"End playing !";
}

void ProcessingThread::chebyFiltering(short int *in_buffer, short int *out_buffer, int vecsize, float sr)
{
    static short int delLeftIn[22];
    static short int delLeftOut[22];

    static short int delRightIn[22];
    static short int delRightOut[22];

    static short int aSumLeft = 0;
    static short int bSumLeft = 0;

    static short int aSumRight = 0;
    static short int bSumRight = 0;

    double a0=0.00511238; //6 pole,  fc= 0.1
    double a1=0.0102248;
    double a2=0.00511238;
    double a3=0.00181723;
    double a4=0.00136292;
    double a5=0.000545169;
    double a6=9.08616e-05;
    double b1=1.79715;
    double b2=-0.817603;
    double b3=9.54371;
    double b4=-6.07937;
    double b5=2.14006;
    double b6=-0.324736;

    if(ind == 1)
    {
        for(int i=0; i<22; i++)
        {
            if(chebyResults->a[i]>0)
                qDebug()<<"a["<<i<<"]="<<chebyResults->a[i];
            if(chebyResults->b[i]>0)
                qDebug()<<"b["<<i<<"]="<<chebyResults->b[i];
        }
        ind = 0;
    }

    for(int i=0; i<vecsize; i = i + 2)
    {
        int part;
        part = 3;
        switch(part)
        {
        case 1://algoritm
        {
            //left channel
            delLeftIn[0] = in_buffer[i];

            aSumLeft = 0;
            for(int j=0; j<=chebyResults->number_of_poles; j++) //a sum
                aSumLeft += chebyResults->a[j]*delLeftIn[j];

            bSumLeft = 0;
            for(int j=1; j<=chebyResults->number_of_poles; j++) //b sum
                bSumLeft += chebyResults->b[j]*delLeftOut[j-1];

            out_buffer[i] = (short int)(aSumLeft + bSumLeft);

            for(int j = chebyResults->number_of_poles; j>=1; j--) //in delay
                delLeftIn[j] = delLeftIn[j-1];

            delLeftOut[0] = out_buffer[i];
            for(int j = chebyResults->number_of_poles-1; j>=1; j--) //out delay
                delLeftOut[j] = delLeftOut[j-1];

            //right channel
            delRightIn[0] = in_buffer[i+1];

            aSumRight = 0;
            for(int j=0; j<=chebyResults->number_of_poles; j++) //a sum
                aSumRight += chebyResults->a[j]*delRightIn[j];

            bSumRight = 0;
            for(int j=1; j<=chebyResults->number_of_poles; j++) //b sum
                bSumRight += chebyResults->b[j]*delRightOut[j-1];

            out_buffer[i+1] = (short int)(aSumRight + bSumRight);

            for(int j = chebyResults->number_of_poles; j>=1; j--) //in delay
                delRightIn[j] = delRightIn[j-1];

            delRightOut[0] = out_buffer[i+1];
            for(int j = chebyResults->number_of_poles-1; j>=1; j--) //out delay
                delRightOut[j] = delRightOut[j-1];
            break;
        }
        case 3: //new algorithm
        {
            //left channel++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
            delLeftIn[0] = in_buffer[i];

            aSumLeft = 0;
            for(int j=0; j<=chebyResults->number_of_poles; j++)
            {
                aSumLeft += chebyResults->a[j]*delLeftIn[j];
                //qDebug()<<"a["<<j<<"]="<<chebyResults->a[j];
            }

            bSumLeft = 0;
            for(int j=1; j<=chebyResults->number_of_poles;j++)
            {
                bSumLeft += chebyResults->b[j]*delLeftOut[j];
                //qDebug()<<"b["<<j<<"]="<<chebyResults->b[j];
            }

            out_buffer[i] = (short int)(aSumLeft  + bSumLeft);

            for(int j=chebyResults->number_of_poles; j>=1; j--)
                delLeftIn[j] = delLeftIn[j-1];

            delLeftOut[1] = out_buffer[i];
            for(int j=chebyResults->number_of_poles; j>1; j--)
                delLeftOut[j] = delLeftOut[j-1];

            //right channel+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

            delRightIn[0] = in_buffer[i+1];

            aSumRight = 0;
            for(int j=0; j<=chebyResults->number_of_poles; j++)
            {
                aSumRight += chebyResults->a[j]*delRightIn[j];
                //qDebug()<<"a["<<j<<"]="<<chebyResults->a[j];
            }

            bSumRight = 0;
            for(int j=1; j<=chebyResults->number_of_poles;j++)
            {
                bSumRight += chebyResults->b[j]*delLeftOut[j];
                //qDebug()<<"b["<<j<<"]="<<chebyResults->b[j];
            }

            out_buffer[i+1] = (short int)(aSumRight  + bSumRight);

            for(int j=chebyResults->number_of_poles; j>=1; j--)
                delRightIn[j] = delRightIn[j-1];

            delRightOut[1] = out_buffer[i+1];
            for(int j=chebyResults->number_of_poles; j>1; j--)
                delRightOut[j] = delRightOut[j-1];
            break;
        }
        case 2://hard
        {
            delLeftIn[0] = in_buffer[i];
            out_buffer[i] = (short int)(a0*delLeftIn[0] + a1*delLeftIn[1] + a2*delLeftIn[2] + b1*delLeftOut[1] + b2*delLeftOut[2]);
            delLeftIn[2] = delLeftIn[1];
            delLeftIn[1] = delLeftIn[0];

            delLeftOut[1] = out_buffer[i];
            delLeftOut[2] = delLeftOut[1];
            break;
        }
        }
        //out_buffer[i+1] = 0;
    }
}

void ProcessingThread::stopProcessingSlot()
{
    stopProcessing = true;
}
