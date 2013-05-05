#include "coefab.h"
#include<QDebug>

void calculateDFT(double *rex, double *imx, int n)
{
    double sr, si;
    double xi[n], xr[n];

    for(int i=0; i<n; i++)
    {
        xi[i] = imx[i];
        xr[i] = rex[i];
    }

    for(int k=0; k<n-1; k++)
    {
        rex[k]=0;
        imx[k]=0;
    }

    for(int k=0; k<n-1; k++)
    {
        for(int i=0; i<n-1; i++)
        {
            sr=cos(2*PI*k*i/n);
            si=-sin(2*PI*k*i/n);
            rex[k]=rex[k]+xr[i]*sr-xi[i]*si;
            imx[k]=imx[k]+xr[i]*si+xi[i]*sr;
        }
    }
}

/*void calculateFFT(double *rex, double *imx, int n) //subroutine 1000
{
    int nm1, nd2, m, j, k, le, le2, ur, ui, jm1, ip;
    double tr, ti, sr, si;

    nm1 = n-1;
    nd2 = n/2;
    m = (log((double)n)/log(2.));

    j = nd2;

    for(int i = 1; i<n-2; i++)
    {
        if(i>j)
            goto line1190;
        else
        {
            tr = rex[j];
            ti = imx[j];
            rex[j] = rex[i];
            imx[j] = imx[i];
            rex[i] = tr;
            imx[i] = ti;
        }
        line1190:
        k=nd2;
        line1200:
        if(k>j)
            goto line1240;
        else
        {
            j=j-k;
            k=k/2;
            goto line1200;
        }
        line1240:
        j=j+k;
    }

    for(int l=1; l<m; l++)
    {
        le=pow(2,l);
        le2=le/2;
        ur=1;
        ui=0;
        sr=cos(PI/(double)le2);
        si=-sin(PI/(double)le2);
        for(j=1; j<le2; j++)
        {
            jm1=j-1;
            for(int i=jm1; i<nm1; i++)
            {
                ip=i+le2;
                tr=rex[ip]*ur-imx[ip]*ui;
                ti=rex[ip]*ui+imx[ip]*ur;
                rex[ip]=rex[i]-tr;
                imx[ip]=imx[i]-ti;
                rex[i]=rex[i]+tr;
                imx[i]=imx[i]+ti;
            }
            tr=ur;
            ur=tr*sr-ui*si;
            ui=tr*si+ui*sr;
        }
    }
}*/

double freqDomainError(double *a, double *b, double *t, double *magDFT, int np, int n)//subroutine 3000
{
    double rex[255];    //real part of signal during FFT
    double imx[255];   //imaginary part of signal during FFT

    double er;
    double mag;
    for(int i=0; i<n-1; i++)  //load shifted impulse into imx[]
    {
        rex[i] = 0;
        imx[i] = 0;
    }
    imx[12] = 1;

    for(int i = 12; i<n-1; i++) //calculate impulse response of current system; result stored in rex
    {
        for(int j=0; j<np; j++)
            rex[i] = rex[i] + a[j] * imx[i-j] + b[j] * rex[i-j];
    }
    imx[12] = 0;

    calculateDFT(rex, imx, n); //calculate frequency response of actual system; rex- impulse response;in imx  = 0, 0, 0, ...

    //calculate the mean squared error between the actual system freq response and
    //the desired frequency response
    er = 0;
    FILE *fileOutDFT=fopen("DFT_out.txt", "w");
    for(int i=0; i<n/2; i++)
    {
        mag = sqrt(rex[i]*rex[i]+imx[i]*imx[i]);
        magDFT[i] = mag;
        fprintf(fileOutDFT, "%f ", mag);
        er = er + (mag-t[i])*(mag-t[i]);
    }
    fclose(fileOutDFT);
    er = sqrt(er/(n/2.+1.));

    return er;
}

void calcNewCoef(double *a, double *b, double *t, double *magDFT, double delta, double mu, double *enew, double *eold, int np, int n)//subroutine 2000
{
    double er;

    double sa[np], sb[np];

    *eold = freqDomainError(a, b, t, magDFT, np, n);

    for(int i=0; i<np; i++) //calculates the slope for a coefficients, stores them to sa
    {
        a[i] = a[i] + delta;
        er = freqDomainError(a, b, t, magDFT, np, n);
        sa[i] = (er- *eold)/delta;
        a[i] = a[i] - delta;
    }

    for(int i=1; i<np; i++)//calculates the slopes for b coefficients
    {
        b[i] = b[i] + delta;
        er = freqDomainError(a, b, t, magDFT, np, n);
        sb[i] = (er- *eold)/delta;
        b[i] = b[i] - delta;
    }

    for(int i=0; i<np; i++)//modify recursion coefficients by an amount proportional to its slope
    {
        a[i] = a[i] - sa[i] * mu;
        b[i] = b[i] - sb[i] * mu;
    }
    *enew = freqDomainError(a, b, t, magDFT, np, n);
    qDebug()<<"enew = "<<*enew;
}
