#ifndef RESULTS_H
#define RESULTS_H

#include"includes.h"

typedef struct complex
{
    double rp;
    double ip;
} complex_number;

/*
 *
 */
typedef struct resultsChebyshev
{
    double *a;              //a coefficients
    double *b;              //b couefficients
    complex_number *pole;   //poles of the filter
    int number_of_poles;
    QString FilePath;

    resultsChebyshev()
    {
        a = new double [22];
        b = new double [22];
        pole = new complex_number [20];
    }

    /*~resultsChebyshev()
    {
        delete [] a;
        delete [] b;
        delete [] pole;
    }*/
}ChebyshevFilterResults;

#endif // RESULTS_H
