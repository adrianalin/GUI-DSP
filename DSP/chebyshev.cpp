#include "chebyshev.h"
#include<qdebug.h>

Chebyshev::Chebyshev()
{
    memset((double*)a, 0, sizeof(double)*22);
    memset((double*)b, 0, sizeof(double)*22);
    memset((double*)ta, 0, sizeof(double)*22);
    memset((double*)tb, 0, sizeof(double)*22);

    for(int i=0; i<20; i++)
    {
        poles[i].rp = 0;
        poles[i].ip = 0;
    }

    a[2] = 1;
    b[2] = 1;
}

void Chebyshev::ComputeStageCoef(double fc, int lh, double pr, int np, int p) //SUB 1000
{
    double rp=0, ip=0, es=0, vx=0, kx=0, t=0, w=0, m=0, d=0, x0=0, x1=0, x2=0, y1=0, y2=0, k=0;        //calculate pole location on the unit circle

    rp = -cos((PI/(np*2.)) + (double)(p-1)*(PI/(double)np)); //real part and imaginary part of pole location (on unit circle)
    ip = sin(PI/(np*2.) + (double)(p-1)*(PI/(double)np)); //calculated for low pass butterworth, at cutoff freq w=1

    if(pr==0) goto label; // Chebysev filter --   0<pr<30

    es = sqrt(pow((100/(100-pr)), 2) - 1);
    vx = (1./np) * log( (1./es) + sqrt(1./(es*es)+1));
    kx = (1./np) * log( (1./es) + sqrt(1./(es*es)-1));
    kx = (exp(kx) + exp(-kx))/2.;
    rp = rp * ((exp(vx) - exp(-vx))/2.)/kx; //pole location on elliptical pattern (warp from circle to ellipse)
    ip = ip * ((exp(vx) + exp(-vx))/2.)/kx;

label:
    poles[p].rp = rp; //s plane poles
    poles[p].ip = ip;

    t = 2.*tan(1./2.);     //convert pole-zero pattern from s-domain to z-domain using bilinear transform
    w = 2.*PI*fc;
    m = rp*rp + ip*ip;
    d = 4. - 4.*rp*t + m*t*t;
    x0 = t*t/d;//recurssion coefficients for one stage of a low-pass filter with a cutoff frequency of 1
    x1 = t*t*2./d;
    x2 = t*t/d;
    y1 = (8. - 2.*m*t*t)/d;
    y2 = (-4.-4.*rp*t-m*t*t)/d;

    //if needed, change from low pass, to high pass
    if(lh == 1)  //LP to LP, or LP to HP transform
        k = -cos(w/2.+1./2.) / cos(w/2.-1./2.);

    if(lh == 0)
        k = sin(1./2.-w/2.) / sin(1./2.+w/2.);

    //change filter cutoff frequency according to user's needs (initially is for w=1)
    d = 1. + y1*k - y2*k*k;
    a0 = (x0 - x1*k + x2*k*k) / d;
    a1 = (-2*x0*k + x1 + x1*k*k - 2*x2*k)/d;
    a2 = (x0*k*k -x1*k + x2)/d;
    b1 = (2*k + y1 + y1*k*k - 2*y2*k)/d;
    b2 = (-(k*k) - y1*k + y2)/d;

    if(lh == 1)
        a1 = -a1;

    if(lh == 1)
        b1 = -b1;
}

void Chebyshev::ComputeCoef(double fc, int lh, double pr, int np)
{
    double sa = 0, sb = 0, gain;

    memset((double*)a, 0, sizeof(double)*22);
    memset((double*)b, 0, sizeof(double)*22);
    memset((double*)ta, 0, sizeof(double)*22);
    memset((double*)tb, 0, sizeof(double)*22);

    a[2] = 1;
    b[2] = 1;

    for(int p=1; p<=np/2; p++) //loop for each pole pair
    {
        ComputeStageCoef(fc, lh, pr, np, p); //GOSUB 1000

        for(int i=0; i<22; i++) //add coefficients to the cascade
        {
            ta[i]=a[i];
            tb[i]=b[i];
        }

        for(int i=2; i<22; i++)
        {
            a[i] = a0*ta[i] + a1*ta[i-1] + a2*ta[i-2];
            b[i] = tb[i] - b1*tb[i-1] - b2*tb[i-2];
        }
    }

    b[2] = 0;  //finish combining coefficients
    for(int i=0; i<20; i++)
    {
        a[i]=a[i+2];
        b[i]=-b[i+2];
    }

    sa=0;    //normalize the gain
    sb=0;
    for(int i=0; i<20; i++)
    {
        if(lh == 0) //for lowpass filter
            sa = sa + a[i];
        if(lh == 0)
            sb = sb + b[i];

        if(lh == 1) //for high pass filter
            sa = sa + a[i]*pow((-1), i);
        if(lh == 1)
            sb = sb + b[i]*pow((-1), i);
    }

    gain = sa/(1-sb);

    for(int i=0; i<20; i++)
        a[i] = a[i]/gain;

    //showCoefficients();
    DisplayPoles();
}

void Chebyshev::showCoefficients()
{
    for(int i=0;i<22;i++)
        if(a[i]!=0)
            qDebug()<<"a["<<i<<"]="<<a[i];

    for(int i=0; i<22; i++)
        if(b[i]!=0)
            qDebug()<<"b["<<i<<"]="<<b[i];

    qDebug()<<endl<<endl;
}

double* Chebyshev::getACoef()
{
    return &a[0];
}

double* Chebyshev::getBCoef()
{
    return &b[0];
}

complex_number* Chebyshev::getPoles()
{
    return &poles[0];
}

void Chebyshev::DisplayPoles()
{
    qDebug()<<"Original step: ";

    for(int i = 1; i<20; i++)
    {
        if((poles[i].rp != 0) && (poles[i].ip != 0))
        {
            qDebug()<<"poles["<<i<<"].rp="<<poles[i].rp;
            qDebug()<<"poles["<<i<<"].ip="<<poles[i].ip;
        }
    }
    qDebug()<<endl<<endl;
}
