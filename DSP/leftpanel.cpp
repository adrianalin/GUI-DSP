#include "leftpanel.h"
#include <qlabel.h>
#include <qcombobox.h>
#include <qspinbox.h>
#include <qcheckbox.h>
#include <qlayout.h>
#include <qdebug.h>
#include <QRadioButton>
#include <QGridLayout>
#include <QSpacerItem>
#include <QThread>
#include <QMessageBox>
#include "qmainwindow.h"
#include "coefab.h"
#include <QApplication>
#include <time.h>

LeftPanel::LeftPanel( QWidget *parent ): QTabWidget( parent )
{
    idealFreq = new double[128];
    acoef = new double[14];
    bcoef = new double[14];

    setTabPosition( QTabWidget::West );

    addTab( createOpenFileTab( this ), "Deschidere fisier" );
    addTab( createFilterTab( this ), "Filtru" );

    connect(sliderCutoffFreq, SIGNAL(sliderMoved(double)), this, SLOT(editedParameters()));
    connect(sliderNumberOfPoles, SIGNAL(sliderMoved(double)), this, SLOT(editedParameters()));
    connect(sliderStage, SIGNAL(sliderMoved(double)), this, SLOT(editedParameters()));
    connect(ComboFilterType, SIGNAL( currentIndexChanged( int ) ), SLOT(editedParameters()));

    connect(ButtonCalculateCoefficients, SIGNAL(clicked()), this, SLOT(calculateCoefficients()));

    connect(ButtonOpen, SIGNAL(clicked()), this, SLOT(OpenFileSlot()));

    connect(ButtonStartProcessing, SIGNAL(clicked()), this, SLOT(startProcessingWAVFileSlot()));

    connect(ButtonStopProcessing, SIGNAL(clicked()), this, SLOT(stopProcessingWAVFileSlot()));

    ButtonStopProcessing->setEnabled(false);

    editedParameters();
}

/*
 *Creez sliderele si le aplic pe tabul filter
 */
QWidget *LeftPanel::createFilterTab( QWidget *parent )
{
    QWidget *page = new QWidget( parent );
    QGridLayout *MainLayout = new QGridLayout( page );

    sliderCutoffFreq = new QwtSlider(Qt::Horizontal);
    sliderCutoffFreq->setGroove(true);
    sliderCutoffFreq->setScale(0, 127);
    sliderCutoffFreq->setTotalSteps(127);
    LineEditCutoffFreq = new QLineEdit("1");
    LineEditCutoffFreq->setReadOnly(true);

    sliderNumberOfPoles = new QwtSlider(Qt::Horizontal);
    sliderNumberOfPoles->setGroove(true);
    sliderNumberOfPoles->setScale(0, 14);
    sliderNumberOfPoles->setTotalSteps(14);
    LinEditNumberOfPoles = new QLineEdit("2");
    LinEditNumberOfPoles->setReadOnly(true);

    sliderStage = new QwtSlider(Qt::Horizontal);
    sliderStage->setGroove(true);
    sliderStage->setScale(0, 10);
    sliderStage->setTotalSteps(10);
    LineEditStage = new QLineEdit("1");
    LineEditStage->setReadOnly(true);

    ComboFilterType = new QComboBox( page );
    ComboFilterType->addItem( "Trece jos" );
    ComboFilterType->addItem( "Trece sus" );

    progressBar = new QProgressBar();
    progressBar->setMinimum(0);
    progressBar->setMaximum(99);

    ButtonStartProcessing = new QPushButton("Start");
    ButtonStopProcessing = new QPushButton("Stop");
    ButtonCalculateCoefficients = new QPushButton("Calculeaza");

    //++++++++++++++frecv de taiere+++++++++++++++++
    QGridLayout *CutoffFreqLayout = new QGridLayout();
    CutoffFreqLayout->addWidget(new QLabel("Frecventa de taiere"), 0, 0);
    CutoffFreqLayout->addWidget(LineEditCutoffFreq, 0, 1);
    CutoffFreqLayout->addWidget(new QLabel("x Fs"), 0, 2);

    MainLayout->addLayout(CutoffFreqLayout, 0, 0);
    MainLayout->addWidget(sliderCutoffFreq, 1, 0);

    //++++++++++++++++++numarul de poli++++++++++++++
    QGridLayout *PolesLayout = new QGridLayout();
    PolesLayout->addWidget(new QLabel("Numarul de poli"), 0, 0);
    PolesLayout->addWidget(LinEditNumberOfPoles, 0, 1);

    MainLayout->addLayout(PolesLayout, 4, 0);
    MainLayout->addWidget(sliderNumberOfPoles, 5, 0);

    //+++++++++++++++++++stagiul filtrului+++++++++++
    QGridLayout *stageLayout = new QGridLayout();
    stageLayout->addWidget(new QLabel("Stagiu"), 0, 0);
    stageLayout->addWidget(LineEditStage, 0, 1);

    MainLayout->addLayout(stageLayout, 6, 0);
    MainLayout->addWidget(sliderStage, 7, 0);

    //+++++++++++++++++tipul filtrului+++++++++++++++
    QGridLayout *FilterType = new QGridLayout();
    FilterType->addWidget(new QLabel("Tipul filtrului", 0, 0));
    FilterType->addWidget(ComboFilterType, 0, 1);
    MainLayout->addLayout(FilterType, 8, 0);

    //++++++++++++++++calculeaza coeficienti++++++++++
    QGridLayout *calcCoef = new QGridLayout();
    calcCoef->addWidget(ButtonCalculateCoefficients, 0, 0);
    calcCoef->addWidget(progressBar, 0, 1);

    MainLayout->addLayout(calcCoef, 9, 0);

    //+++++++++++++++++butoanele starts si stop++++++++
    QGridLayout *startStop = new QGridLayout();
    startStop->addWidget(ButtonStartProcessing, 0,2);
    startStop->addWidget(ButtonStopProcessing, 0, 3);

    MainLayout->addLayout(startStop, 10, 0);

    //MainLayout->addItem(spacer1, 11, 0);

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

/*
 *apelat de fiecare data cand utilizatorul muta un slider
 */
void LeftPanel::editedParameters()
{
    //+++++++++++++++++++++++++++preiau setarile din interfata grafica+++++++++++++++++
    if(sliderCutoffFreq->value()>0)
        LineEditCutoffFreq->setText(QString::number(sliderCutoffFreq->value())); //scriu in textbox

    if(sliderNumberOfPoles->value()>=2)
        LinEditNumberOfPoles->setText(QString::number(sliderNumberOfPoles->value()));

    if(sliderStage->value()>=1)
        LineEditStage->setText(QString::number(sliderStage->value()));

    for(int i=0; i<128; i++)
    {
        switch(ComboFilterType->currentIndex())
        {
        case 0:
        {
            for(int i=0; i<128; i++)
                if(i<LineEditCutoffFreq->text().toInt())
                    idealFreq[i] = 0.9;
                else
                    idealFreq[i] = 0;
            break;
        }
        case 1:
        {
            for(int i=0; i<128; i++)
                if(i<LineEditCutoffFreq->text().toInt())
                    idealFreq[i] = 0;
                else
                    idealFreq[i] = 0.9;
            break;
        }
        default:
        {

        }
        }
    }

    Q_EMIT plotIdealFilter(idealFreq);
}

void LeftPanel::calculateCoefficients()
{
    clock_t start, stop;         //masor timpul necesar pentru calculul coeficientilor filtrului
    double exec_time;            //http://stackoverflow.com/questions/5248915/execution-time-of-c-program

    start = clock();

    ButtonCalculateCoefficients->setEnabled(false);

    int n = 256;                                //algoritmul foloseşte FFT de lungime 256
    FILE *fileCoeff=fopen("out_coef.txt", "w"); //scriu rezultatele in fisiere text, pentru a le putea interpreta in matlab
    FILE *fileInDFT=fopen("DFT_in.txt", "w");
    double delta = .00001;                      //perturbation increment
    double mu = .2;                             //iteration step size
    double magDFT[256];                         //este reprezentat grafic
    double enew, eold;
    int np = LinEditNumberOfPoles->text().toInt();  //numarul de poli = max 14

    for(int i=0; i<128; i++)
        fprintf(fileInDFT, "%f ", idealFreq[i]);
    fclose(fileInDFT);

    for(int i=0; i<np; i++)                        //setez coeficientii la valoarea initiala- sistemul identitate
    {
        acoef[i] = 0;
        bcoef[i] = 0;
    }
    acoef[0] = 1;

    for(int i=1; i<90; i++)
    {
        progressBar->setValue(i);
        calcNewCoef(acoef, bcoef, idealFreq, magDFT, delta, mu, &enew, &eold, np, n);
        qApp->processEvents();
        Q_EMIT plotRealFilter(magDFT);
        if(enew>eold)
            mu = mu/2;                             //adjust iteration step size
    }
    progressBar->setValue(0);

    fprintf(fileCoeff, "a: ");
    qDebug()<<"\n";
    for(int i = 0; i<np; i++)
    {
        fprintf(fileCoeff, "%f ", acoef[i]);
    }
    fprintf(fileCoeff, "\n b: ");
    for(int i = 1; i<np; i++)
    {
        fprintf(fileCoeff, "%f ", bcoef[i]);
    }
    fclose(fileCoeff);

    stop = clock();
    exec_time = (double)(stop - start) / CLOCKS_PER_SEC; //calculez timpul necesar pentru calculul coeficientilor

    ButtonCalculateCoefficients->setEnabled(true);
    Q_EMIT showTimeSpentAndCoefficients(exec_time ,acoef, bcoef, np);
}

void LeftPanel::OpenFileSlot()
{
    QString filePath;
    filePath = QFileDialog::getOpenFileName(this, tr("Open File"), "/home/adrian/Sounds/", tr("Files (*.wav)"));
    LineEditFilePath->setText(filePath);
    if(!filePath.isNull())
        Q_EMIT openedWAVFile(filePath);
}

/*
 *pornesc thread separat pentru procesare de sunet;
 *daca nu folosesc un thread separat, se blocheaza interfata grafica
 */
void LeftPanel::startProcessingWAVFileSlot()
{
    if(LineEditFilePath->text() == NULL)
    {
        QMessageBox::warning(this, tr("DSP"), tr("Selectati un fisier .WAV!"));
        return ;
    }

    QThread *thread = new QThread;
    worker = new ProcessingThread(acoef, bcoef, LineEditFilePath->text(), LinEditNumberOfPoles->text().toInt(), LineEditStage->text().toInt());

    ButtonStartProcessing->setEnabled(false);
    ButtonStopProcessing->setEnabled(true);

    worker->moveToThread(thread);
    connect(thread, SIGNAL(started()), worker, SLOT(startProcessing()));
    connect(worker, SIGNAL(finished()), thread, SLOT(quit()));
    connect(worker, SIGNAL(finished()), this, SLOT(enableStartDisableStopButton()));
    connect(worker, SIGNAL(finished()), worker, SLOT(deleteLater()));
    connect(thread, SIGNAL(finished()), thread, SLOT(deleteLater()));
    thread->start();
}

void LeftPanel::enableStartDisableStopButton()
{
    ButtonStartProcessing->setEnabled(true);
    ButtonStopProcessing->setEnabled(false);
}

void LeftPanel::stopProcessingWAVFileSlot()
{
    worker->stopProcessingSlot();
}
