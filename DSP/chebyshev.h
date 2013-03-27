#ifndef CHEBYSHEV_H
#define CHEBYSHEV_H

#include"includes.h"
#include "results.h"

/*
 *"The scientist and engineer's guide to digital signal processing" (pag 340)
 *-best explained in chapter 33 (pag 623)
 */

class Chebyshev
{
private:
    double a0, a1, a2, b1, b2;
    complex_number poles[20];
    double a[22], b[22]; //output coefficients
    double ta[22], tb[22];
    void ComputeStageCoef(double fc, int lh, double pr, int np, int p);
    void showCoefficients();
    void DisplayPoles();

public:
    Chebyshev();

    /*fc-  cutoff freq [0-0.5] expressed as a fraction of the sampling freq
    *lh- lh=0 low pass, lh=1 high pass,
    *pr- ripple[0..29%]
    *np- number of poles [2 4 .. 20]*/
    void ComputeCoef(double fc , int lh, double pr, int np);
    double* getACoef();
    double* getBCoef();
    complex_number* getPoles();
};

#endif // CHEBYSHEV_H
