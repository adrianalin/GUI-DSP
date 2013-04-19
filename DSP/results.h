#ifndef RESULTS_H
#define RESULTS_H
#include <QString>

typedef struct complex
{
    double rp;
    double ip;
}complex_number;

class ChebyshevFilterResults
{
public:
    double *a;              //a coefficients
    double *b;              //b couefficients
    complex_number *pole;   //poles of the filter
    int number_of_poles;
    QString FilePath;

    ChebyshevFilterResults()
    {
        a = new double[22];
        b = new double[22];

        for(int i=0; i<22;i++)
        {
            a[i] = 0;
            b[i] = 0;
        }

        pole = new complex_number[20];
    }

    ~ChebyshevFilterResults()
    {
        delete [] a;
        delete [] b;
        delete [] pole;
    }
};

#endif // RESULTS_H
