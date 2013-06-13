#include "coefab.h"
#include<fftw3.h>
#include<QDebug>

/*
 *algoritm ineficient
 */
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

double freqDomainError(double *a, double *b, double *t, double *magDFT, int np, int n)//subroutine 3000
{
    double rex[255];    //real part of signal during FFT
    double imx[255];   //imaginary part of signal during FFT

    double er=0;
    double mag=0;
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
    for(int i=0; i<n-1; i++)
    {
        mag = sqrt(rex[i]*rex[i]+imx[i]*imx[i]);
        magDFT[i] = mag;
        if(i<n/2)
        {
            fprintf(fileOutDFT, "%f ", mag);
            er = er + (mag-t[i])*(mag-t[i]);
        }
    }

    fclose(fileOutDFT);
    er = sqrt(er/(n/2.+1.));

    return er;
}

void calcNewCoef(double *a, double *b, double *t, double *magDFT, double delta, double mu, double *enew, double *eold, int np, int n)//subroutine 2000
{
    double er=0;

    double sa[np], sb[np];

    for(int i=0; i<np; i++)
    {
        sa[i] = 0;
        sb[i] = 0;
    }

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
