#include "mainwindow.h"
#include "ui_mainwindow.h"

#include "segmentview.h"

#include <QHBoxLayout>
#include <QListWidget>
#include <QFileDialog>
#include <QGroupBox>
#include <QLabel>

extern "C"
{
#include "sc-store/sc_store.h"
}

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    mSegmentsList(0),
    mSegmentView(0),
    mLabelNodeCount(0),
    mLabelArcsCount(0),
    mLabelEmptyCount(0)
{
    ui->setupUi(this);

    createMainWidgets();
    setupMenuActions();
}


void MainWindow::createMainWidgets()
{
    QWidget *central = new QWidget();
    QHBoxLayout *topLayout = new QHBoxLayout();
    QVBoxLayout *mainLayout = new QVBoxLayout();

    mSegmentsList = new QListWidget(central);
    mSegmentsList->setMaximumWidth(200);

    connect(mSegmentsList, SIGNAL(currentTextChanged(QString)), this, SLOT(segmentSelectionChanged(QString)));

    mSegmentView = new SegmentView(central);

    QGroupBox *segmentInfoGroup = new QGroupBox(tr("Segment info"), central);
    QVBoxLayout *infoLayout = new QVBoxLayout(central);

    mLabelNodeCount = new QLabel(tr("Nodes: "), central);
    mLabelArcsCount = new QLabel(tr("Arcs: "), central);
    mLabelEmptyCount = new QLabel(tr("Empty: "), central);

    infoLayout->addWidget(mLabelNodeCount);
    infoLayout->addWidget(mLabelArcsCount);
    infoLayout->addWidget(mLabelEmptyCount);

    segmentInfoGroup->setLayout(infoLayout);

    topLayout->addWidget(mSegmentsList, 0, Qt::AlignLeft);
    topLayout->addWidget(segmentInfoGroup, 2);

    mainLayout->addLayout(topLayout);
    mainLayout->addWidget(mSegmentView, 2);

    central->setLayout(mainLayout);
    setCentralWidget(central);
}

void MainWindow::setupMenuActions()
{
    connect(ui->actionOpen, SIGNAL(triggered()), this, SLOT(openRepository()));
    connect(ui->actionExit, SIGNAL(triggered()), this, SLOT(close()));
}

MainWindow::~MainWindow()
{
    delete ui;

    delete mSegmentsList;
    delete mSegmentView;
}

void MainWindow::updateSegmentsList()
{
    Q_ASSERT(sc_storage_is_initialized());
    mSegmentsList->clear();

    quint32 segmentsCount = sc_storage_get_segments_count();
    for (quint32 seg = 0; seg < segmentsCount; seg++)
    {
        sc_segment *segment = sc_storage_get_segment(seg, SC_TRUE);
        Q_ASSERT(segment != 0);

        mSegmentsList->addItem(QString("%1").arg(seg));
    }
}


void MainWindow::openRepository()
{
    QString path = QFileDialog::getExistingDirectory(this, tr("Choose repository"));

    if (path.length() > 0)
    {
        if (sc_storage_is_initialized())
            sc_storage_shutdown();

        sc_storage_initialize(path.toStdString().c_str());
        updateSegmentsList();
    }
}

void MainWindow::segmentSelectionChanged(QString strId)
{
    if (strId.isEmpty())
    {
        mSegmentView->reset();
    }else
    {
        int id = strId.toInt();
        mSegmentView->setSegmentId(id);
    }
}

