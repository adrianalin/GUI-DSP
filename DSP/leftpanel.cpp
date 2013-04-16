#include "leftpanel.h"
#include <qlabel.h>
#include <qcombobox.h>
#include <qspinbox.h>
#include <qcheckbox.h>
#include <qlayout.h>
#include <qwt_plot_curve.h>
#include <qdebug.h>
#include <QRadioButton>
#include <QGridLayout>
#include <QSpacerItem>
#include "qmainwindow.h"

LeftPanel::LeftPanel( QWidget *parent ): QTabWidget( parent )
{
    chebyResults = new ChebyshevFilterResults;

    setTabPosition( QTabWidget::West );

    addTab( createOpenFileTab( this ), "Deschidere fisier" );
    addTab( createChebyshevTab( this ), "Chebyshev" );
    addTab( createCurveTab( this ), "Curve" );

    chebyInputSettings = new ChebySettings; //salvez setarile introduse de utilizator din interfata grafica

    connect( chebyControls.sliderCutoffFreq, SIGNAL(sliderMoved(double)), this, SLOT(editedChebyshevParameters()));
    connect( chebyControls.sliderRipple, SIGNAL(sliderMoved(double)), this, SLOT(editedChebyshevParameters()));
    connect( chebyControls.sliderNumberOfPoles, SIGNAL(sliderMoved(double)), this, SLOT(editedChebyshevParameters()));

    connect(chebyControls.sliderCutoffFreq,SIGNAL(sliderReleased()), this, SLOT(startProcessingChebyshevParameters()));
    connect(chebyControls.sliderRipple, SIGNAL(sliderReleased()), this, SLOT(startProcessingChebyshevParameters()));
    connect(chebyControls.sliderNumberOfPoles, SIGNAL(sliderReleased()), this, SLOT(startProcessingChebyshevParameters()));

    connect( chebyControls.d_filterType, SIGNAL( currentIndexChanged( int ) ), SLOT(startProcessingChebyshevParameters()));

    connect(ButtonOpen, SIGNAL(released()), this, SLOT(OpenFileSlot()));

    connect(ButtonStartProcessing, SIGNAL(released()), this, SLOT(startProcessingSlot()));

    editedChebyshevParameters();
    startProcessingChebyshevParameters();
}

/*
 *Creez sliderele si le aplica pe tabul Chebyshev
 */
QWidget *LeftPanel::createChebyshevTab( QWidget *parent )
{
    QWidget *page = new QWidget( parent );
    QGridLayout *MainLayout = new QGridLayout( page );

    chebyControls.sliderCutoffFreq = new QwtSlider(Qt::Horizontal);
    chebyControls.sliderCutoffFreq->setGroove(true);
    chebyControls.sliderCutoffFreq->setScale(0, 0.5);
    chebyControls.sliderCutoffFreq->setTotalSteps(20);
    chebyControls.d_cutoffFreq = new QLineEdit("0.25");
    chebyControls.d_cutoffFreq->setReadOnly(true);

    chebyControls.sliderRipple = new QwtSlider(Qt::Horizontal);
    chebyControls.sliderRipple->setGroove(true);
    chebyControls.sliderRipple->setScale(0, 30);
    chebyControls.sliderRipple->setTotalSteps(300);
    chebyControls.d_ripple = new QLineEdit("0");
    chebyControls.d_ripple->setReadOnly(true);

    chebyControls.sliderNumberOfPoles = new QwtSlider(Qt::Horizontal);
    chebyControls.sliderNumberOfPoles->setGroove(true);
    chebyControls.sliderNumberOfPoles->setScale(0, 20);
    chebyControls.sliderNumberOfPoles->setTotalSteps(10);
    chebyControls.d_numberOfPoles = new QLineEdit("2");
    chebyControls.d_numberOfPoles->setReadOnly(true);

    chebyControls.d_filterType = new QComboBox( page );
    chebyControls.d_filterType->addItem( "Trece jos" );
    chebyControls.d_filterType->addItem( "Trece sus" );

    ButtonStartProcessing = new QPushButton("Start");

    //++++++++++++++frecv de taiere+++++++++++++++++
    QGridLayout *CutoffFreqLayout = new QGridLayout();
    CutoffFreqLayout->addWidget(new QLabel("Frecventa de taiere"), 0, 0);
    CutoffFreqLayout->addWidget(chebyControls.d_cutoffFreq, 0, 1);
    CutoffFreqLayout->addWidget(new QLabel("x Fs"), 0, 2);

    MainLayout->addLayout(CutoffFreqLayout, 0, 0);
    MainLayout->addWidget(chebyControls.sliderCutoffFreq, 1, 0);

    //+++++++++++++++riplul+++++++++++++++++++++++++
    QGridLayout *RippleLayout = new QGridLayout();
    RippleLayout->addWidget(new QLabel( "Riplu"), 0, 0);
    RippleLayout->addWidget(chebyControls.d_ripple, 0, 1);

    MainLayout->addLayout(RippleLayout,2, 0);
    MainLayout->addWidget(chebyControls.sliderRipple, 3, 0);

    //++++++++++++++++++numarul de poli++++++++++++++
    QGridLayout *PolesLayout = new QGridLayout();
    PolesLayout->addWidget(new QLabel("Numarul de poli"), 0, 0);
    PolesLayout->addWidget(chebyControls.d_numberOfPoles, 0, 1);

    MainLayout->addLayout(PolesLayout, 4, 0);
    MainLayout->addWidget(chebyControls.sliderNumberOfPoles, 5, 0);

    //+++++++++++++++++tipul filtrului+++++++++++++++
    QGridLayout *FilterType = new QGridLayout();
    FilterType->addWidget(new QLabel("Tipul filtrului", 0, 0));
    FilterType->addWidget(chebyControls.d_filterType, 0, 1);
    FilterType->addWidget(ButtonStartProcessing, 0,2);

    MainLayout->addLayout(FilterType, 6, 0);

    page->setLayout(MainLayout);

    return page;
}

QWidget *LeftPanel::createOpenFileTab( QWidget *parent )
{
    QWidget *page = new QWidget( parent );

    QSpacerItem *spacer1 = new QSpacerItem(0, 0, QSizePolicy::Minimum, QSizePolicy::Expanding);
    ButtonOpen = new QPushButton("Deschide fisier");
    LineEditFilePath = new QLineEdit();
    LineEditFilePath->setReadOnly(true);

    QGridLayout *MainLayout = new QGridLayout;
    MainLayout->addWidget(ButtonOpen, 0, 0);
    MainLayout->addWidget(LineEditFilePath, 0, 1);
    MainLayout->addItem(spacer1, 4, 0);

    page->setLayout(MainLayout);

    return page;
}

QWidget *LeftPanel::createCurveTab( QWidget *parent )
{
    QWidget *page = new QWidget( parent );
    return page;
}

/*
 *apelat de fiecare data cand utilizatorul muta un slider
 */
void LeftPanel::editedChebyshevParameters()
{
    //+++++++++++++++++++++++++++preiau setarile din interfata grafica+++++++++++++++++
    if(chebyControls.sliderCutoffFreq->value()>0)
    {
        chebyInputSettings->cutoffFreq = chebyControls.sliderCutoffFreq->value(); //iau valoarea de la slider
        chebyControls.d_cutoffFreq->setText(QString::number(chebyInputSettings->cutoffFreq)); //o scriu in textbox
    }

    if(chebyControls.sliderRipple->value()<29)
    {
        chebyInputSettings->ripple = chebyControls.sliderRipple->value();
        chebyControls.d_ripple->setText(QString::number(chebyInputSettings->ripple));
    }

    if(chebyControls.sliderNumberOfPoles->value()>=2)
    {
        chebyInputSettings->numPoles = chebyControls.sliderNumberOfPoles->value();
        chebyControls.d_numberOfPoles->setText(QString::number(chebyInputSettings->numPoles));
    }
    chebyInputSettings->filterType = chebyControls.d_filterType->currentIndex();
}


/*
 *Apelat de fiecare data cand utilizatorul elibereaza un slider
 *Aici incep procesarea setarilor date de utilizator si rezultatul il trimit la rightpannel
 */
void LeftPanel::startProcessingChebyshevParameters()
{
    Chebyshev chebyFilter; //implementarea filtrului

    chebyFilter.ComputeCoef(chebyInputSettings->cutoffFreq, chebyInputSettings->filterType, chebyInputSettings->ripple, chebyInputSettings->numPoles);

    chebyResults->a = chebyFilter.getACoef();
    chebyResults->b = chebyFilter.getBCoef();
    chebyResults->pole = chebyFilter.getPoles();
    chebyResults->number_of_poles = chebyInputSettings->numPoles;

    Q_EMIT settingsProcessed(*chebyResults);//trimit rezultatele la rightpannel pentru afisare
}

void LeftPanel::OpenFileSlot()
{
    chebyResults->FilePath = QFileDialog::getOpenFileName(this, tr("Open File"), "/home/adrian/Sounds/", tr("Files (*.wav)"));
    LineEditFilePath->setText(chebyResults->FilePath);
    Q_EMIT openedWAVFile(chebyResults->FilePath);
}

void LeftPanel::startProcessingSlot()
{
    Q_EMIT startProcessingWAVFile();
}
