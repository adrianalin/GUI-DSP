#include "panel.h"
#include <qlabel.h>
#include <qcombobox.h>
#include <qspinbox.h>
#include <qcheckbox.h>
#include <qlayout.h>
#include <qwt_plot_curve.h>
#include <qdebug.h>
#include <QRadioButton>

Panel::Panel( QWidget *parent ): QTabWidget( parent )
{
    setTabPosition( QTabWidget::West );

    addTab( createChebyshevTab( this ), "Chebyshev" );
    addTab( createCanvasTab( this ), "Canvas" );
    addTab( createCurveTab( this ), "Curve" );

    connect( chebyControls.d_cutoffFreq, SIGNAL(editingFinished()), SLOT( editedChebyshevParameters()) );
    connect( chebyControls.d_ripple, SIGNAL(editingFinished()), SLOT( editedChebyshevParameters()) );
    connect( chebyControls.d_numberOfPoles, SIGNAL(editingFinished()), SLOT( editedChebyshevParameters()) );
    connect( chebyControls.d_filterType, SIGNAL( currentIndexChanged( int ) ), SLOT( editedChebyshevParameters()) );

    editedChebyshevParameters();
}

QWidget *Panel::createChebyshevTab( QWidget *parent )
{
    int row = 0;
    QWidget *page = new QWidget( parent );
    QGridLayout *layout = new QGridLayout( page );

    chebyControls.d_cutoffFreq = new QLineEdit();
    chebyControls.d_ripple = new QLineEdit();
    chebyControls.d_numberOfPoles = new QLineEdit();
    chebyControls.a_chebyshev_coefficients = new QLabel();
    chebyControls.b_chebyshev_coefficients = new QLabel();
    chebyControls.d_filterType = new QComboBox( page );
    chebyControls.d_filterType->addItem( "Lowpass" );
    chebyControls.d_filterType->addItem( "Highpass" );

    chebyControls.d_cutoffFreq->setText("0.01");
    chebyControls.d_ripple->setText("0.5");
    chebyControls.d_numberOfPoles->setText("2");

    layout->addWidget( new QLabel( "Cutoff frequency", page ), row, 0 );
    layout->addWidget( chebyControls.d_cutoffFreq, row, 1 );
    layout->addWidget( new QLabel( "Hz", page ), row++, 2 );

    layout->addWidget( new QLabel( "Ripple", page ), row, 0 );
    layout->addWidget( chebyControls.d_ripple, row, 1 );
    layout->addWidget(new QLabel("%", page), row++, 2);

    layout->addWidget(new QLabel("Number of poles", page), row, 0);
    layout->addWidget(chebyControls.d_numberOfPoles, row++, 1);

    layout->addWidget( new QLabel( "Filter type", page ), row, 0 );
    layout->addWidget( chebyControls.d_filterType, row++, 1 );

    layout->addWidget( new QLabel( "coefficients a:", page ), row, 0 );
    layout->addWidget( chebyControls.a_chebyshev_coefficients, row++, 1);

    layout->addWidget( new QLabel( "coefficients b:", page ), row, 0 );
    layout->addWidget( chebyControls.b_chebyshev_coefficients, row, 1);

    layout->addLayout( new QHBoxLayout(), row++, 0 );

    layout->setColumnStretch( 1, 10 );
    layout->setRowStretch( row, 10 );

    return page;
}

QWidget *Panel::createCanvasTab( QWidget *parent )
{
    QWidget *page = new QWidget( parent );

    return page;
}

QWidget *Panel::createCurveTab( QWidget *parent )
{
    QWidget *page = new QWidget( parent );
    return page;
}

void Panel::editedChebyshevParameters()// apelat de fiecare data cand utilizatorul schimba ceva in interfata
{
    QString str;
    ChebySettings s;

    str = chebyControls.d_cutoffFreq->text();
    s.cutoffFreq = str.toDouble();

    str = chebyControls.d_ripple->text();
    s.ripple = str.toDouble();

    str = chebyControls.d_numberOfPoles->text();
    s.numPoles = str.toInt();

    s.filterType = chebyControls.d_filterType->currentIndex();

    processChebyshevParameters(s);
}

void Panel::processChebyshevParameters(ChebySettings s) //aplic algoritmul pentru determinarea coeficientilor
{
    Chebyshev chebyFilter;
    ChebyshevFilterResults chebyResults;

    chebyFilter.ComputeCoef(s.cutoffFreq, s.filterType, s.ripple, s.numPoles);

    chebyResults.a = chebyFilter.getACoef();
    chebyResults.b = chebyFilter.getBCoef();
    chebyResults.pole = chebyFilter.getPoles();
    chebyResults.number_of_poles = s.numPoles;

    displayChebyFilterCoefficients(chebyResults.a, chebyResults.b);

    Q_EMIT settingsProcessed(chebyResults);
}

void Panel::displayChebyFilterCoefficients(double* a, double *b)
{
    QString str;

    for(int i=0;i<22;i++)
    {
        if(a[i]!=0)
            str=str.append(QString("a%1=%2\n").arg(i).arg(a[i]));
    }

    chebyControls.a_chebyshev_coefficients->setText(str);
    str.clear();

    for(int i=0;i<22;i++)
    {
        if(b[i]!=0)
            str=str.append(QString("b%1=%2\n").arg(i).arg(b[i]));
    }
    chebyControls.b_chebyshev_coefficients->setText(str);
}
