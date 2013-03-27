#ifndef _SETTINGS_H_
#define _SETTINGS_H_

#include <qpen.h>
#include <qbrush.h>

class ChebySettings
{
public:
    double cutoffFreq;
    double ripple;
    int numPoles;
    int filterType;

    struct gridSettings
    {
        QPen pen;
    } grid;

    ChebySettings()
    {
        filterType = 0;
        cutoffFreq = 0.01;
        ripple=0.5;
        numPoles=2;
        grid.pen.setStyle(Qt::DashLine);
    }
};

#endif
