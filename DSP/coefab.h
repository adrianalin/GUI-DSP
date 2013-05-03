#ifndef COEFAB_H
#define COEFAB_H

#include<stdio.h>
#include<math.h>

#define PI 3.14159265
void calcNewCoef(double *a, double *b, double *t, double *magDFT, double delta, double mu, double *enew, double *eold, int np, int n);

#endif // COEFAB_H
