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
#include <QThread>
#include <QMessageBox>
#include "qmainwindow.h"

LeftPanel::LeftPanel( QWidget *parent ): QTabWidget( parent )
{
    filteringResults = new ChebyshevFilterResults;

    setTabPosition( QTabWidget::West );

    addTab( createOpenFileTab( this ), "Deschidere fisier" );
    addTab( createChebyshevTab( this ), "Chebyshev" );
    addTab( createCurveTab( this ), "Curve" );

    connect( sliderCutoffFreq, SIGNAL(sliderMoved(double)), this, SLOT(editedChebyshevParameters()));
    connect( sliderRipple, SIGNAL(sliderMoved(double)), this, SLOT(editedChebyshevParameters()));
    connect( sliderNumberOfPoles, SIGNAL(sliderMoved(double)), this, SLOT(editedChebyshevParameters()));

    connect(sliderCutoffFreq,SIGNAL(sliderReleased()), this, SLOT(startProcessingChebyshevParameters()));
    connect(sliderRipple, SIGNAL(sliderReleased()), this, SLOT(startProcessingChebyshevParameters()));
    connect(sliderNumberOfPoles, SIGNAL(sliderReleased()), this, SLOT(startProcessingChebyshevParameters()));

    connect(ComboFilterType, SIGNAL( currentIndexChanged( int ) ), SLOT(startProcessingChebyshevParameters()));

    connect(ButtonOpen, SIGNAL(released()), this, SLOT(OpenFileSlot()));

    connect(ButtonStartProcessing, SIGNAL(released()), this, SLOT(startProcessingWAVFileSlot()));

    connect(ButtonStopProcessing, SIGNAL(released()), this, SLOT(stopProcessingWAVFileSlot()));

    ButtonStopProcessing->setEnabled(false);

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

    sliderCutoffFreq = new QwtSlider(Qt::Horizontal);
    sliderCutoffFreq->setGroove(true);
    sliderCutoffFreq->setScale(0, 0.5);
    sliderCutoffFreq->setTotalSteps(20);
    LineEditCutoffFreq = new QLineEdit("0.25");
    LineEditCutoffFreq->setReadOnly(true);

    sliderRipple = new QwtSlider(Qt::Horizontal);
    sliderRipple->setGroove(true);
    sliderRipple->setScale(0, 30);
    sliderRipple->setTotalSteps(300);
    LineEditRipple = new QLineEdit("0");
    LineEditRipple->setReadOnly(true);

    sliderNumberOfPoles = new QwtSlider(Qt::Horizontal);
    sliderNumberOfPoles->setGroove(true);
    sliderNumberOfPoles->setScale(0, 20);
    sliderNumberOfPoles->setTotalSteps(10);
    LinEditNumberOfPoles = new QLineEdit("2");
    LinEditNumberOfPoles->setReadOnly(true);

    ComboFilterType = new QComboBox( page );
    ComboFilterType->addItem( "Trece jos" );
    ComboFilterType->addItem( "Trece sus" );

    ButtonStartProcessing = new QPushButton("Start");
    ButtonStopProcessing = new QPushButton("Stop");

    //++++++++++++++frecv de taiere+++++++++++++++++
    QGridLayout *CutoffFreqLayout = new QGridLayout();
    CutoffFreqLayout->addWidget(new QLabel("Frecventa de taiere"), 0, 0);
    CutoffFreqLayout->addWidget(LineEditCutoffFreq, 0, 1);
    CutoffFreqLayout->addWidget(new QLabel("x Fs"), 0, 2);

    MainLayout->addLayout(CutoffFreqLayout, 0, 0);
    MainLayout->addWidget(sliderCutoffFreq, 1, 0);

    //+++++++++++++++riplul+++++++++++++++++++++++++
    QGridLayout *RippleLayout = new QGridLayout();
    RippleLayout->addWidget(new QLabel( "Riplu"), 0, 0);
    RippleLayout->addWidget(LineEditRipple, 0, 1);

    MainLayout->addLayout(RippleLayout,2, 0);
    MainLayout->addWidget(sliderRipple, 3, 0);

    //++++++++++++++++++numarul de poli++++++++++++++
    QGridLayout *PolesLayout = new QGridLayout();
    PolesLayout->addWidget(new QLabel("Numarul de poli"), 0, 0);
    PolesLayout->addWidget(LinEditNumberOfPoles, 0, 1);

    MainLayout->addLayout(PolesLayout, 4, 0);
    MainLayout->addWidget(sliderNumberOfPoles, 5, 0);

    //+++++++++++++++++tipul filtrului+++++++++++++++
    QGridLayout *FilterType = new QGridLayout();
    FilterType->addWidget(new QLabel("Tipul filtrului", 0, 0));
    FilterType->addWidget(ComboFilterType, 0, 1);
    FilterType->addWidget(ButtonStartProcessing, 0,2);
    FilterType->addWidget(ButtonStopProcessing, 0, 3);

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
    if(sliderCutoffFreq->value()>0)
        LineEditCutoffFreq->setText(QString::number(sliderCutoffFreq->value())); //o scriu in textbox

    if(sliderRipple->value()<29)
        LineEditRipple->setText(QString::number(sliderRipple->value()));

    if(sliderNumberOfPoles->value()>=2)
        LinEditNumberOfPoles->setText(QString::number(sliderNumberOfPoles->value()));
}


/*
 *Apelat de fiecare data cand utilizatorul elibereaza un slider
 *Aici incep procesarea setarilor date de utilizator si rezultatul il trimit la rightpannel
 */
void LeftPanel::startProcessingChebyshevParameters()
{
    double *acoef;
    double *bcoef;
    complex_number *poli;

    Chebyshev chebyFilter; //implementarea filtrului

    chebyFilter.ComputeCoef((double)(LineEditCutoffFreq->text().toDouble()),
                            (double)(ComboFilterType->currentIndex()),
                            (double)(LineEditRipple->text().toDouble()),
                            (double)(LinEditNumberOfPoles->text().toDouble()));

    acoef = chebyFilter.getACoef();
    bcoef = chebyFilter.getBCoef();
    for(int i = 0; i<22; i++)
    {
        filteringResults->a[i] = acoef[i];
        filteringResults->b[i] = bcoef[i];
    }

    poli=chebyFilter.getPoles();
    filteringResults->pole = poli;

    filteringResults->number_of_poles = (double)LinEditNumberOfPoles->text().toDouble();

    Q_EMIT settingsProcessed(*filteringResults);//trimit rezultatele la rightpannel pentru afisare
}

void LeftPanel::OpenFileSlot()
{
    filteringResults->FilePath = QFileDialog::getOpenFileName(this, tr("Open File"), "/home/adrian/Sounds/", tr("Files (*.wav)"));
    LineEditFilePath->setText(filteringResults->FilePath);
    if(!filteringResults->FilePath.isNull())
        Q_EMIT openedWAVFile(filteringResults->FilePath);
}

/*
 *pornesc thread separat pentru procesare de sunet;
 *daca nu folosesc un thread separat, se blocheaza interfata grafica
 */
void LeftPanel::startProcessingWAVFileSlot()
{
    worker = new ProcessingThread(*filteringResults);

    QThread *thread = new QThread;
    if(filteringResults->FilePath.isNull())
    {
        QMessageBox::warning(this, tr("DSP"), tr("Selectati un fisier .WAV!"));
        return ;
    }

    ButtonStartProcessing->setEnabled(false);
    ButtonStopProcessing->setEnabled(true);

    worker->moveToThread(thread);
    connect(thread, SIGNAL(started()), worker, SLOT(startProcessing()));
    connect(worker, SIGNAL(finished()), thread, SLOT(quit()));
    connect(worker, SIGNAL(finished()), worker, SLOT(deleteLater()));
    connect(thread, SIGNAL(finished()), thread, SLOT(deleteLater()));
    thread->start();
}

void LeftPanel::stopProcessingWAVFileSlot()
{
    ButtonStartProcessing->setEnabled(true);
    ButtonStopProcessing->setEnabled(false);

    worker->stopProcessingSlot();
}
