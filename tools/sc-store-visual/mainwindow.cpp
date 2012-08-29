#include "mainwindow.h"
#include "ui_mainwindow.h"

#include "segmentview.h"

#include <QHBoxLayout>
#include <QListWidget>
#include <QFileDialog>

#include "sc_store.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    mSegmentsList(0),
    mSegmentView(0)
{
    ui->setupUi(this);

    createMainWidgets();
    setupActions();
}


void MainWindow::createMainWidgets()
{
    QWidget *central = new QWidget();
    QHBoxLayout *mainLayout = new QHBoxLayout();

    mSegmentsList = new QListWidget(central);
    mSegmentView = new SegmentView(central);

    mainLayout->addWidget(mSegmentsList);
    mainLayout->addWidget(mSegmentView, 2);

    central->setLayout(mainLayout);
    setCentralWidget(central);
}

void MainWindow::setupActions()
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

void MainWindow::openRepository()
{
    QString path = QFileDialog::getExistingDirectory(this, tr("Choose repository"));

    if (path.length() > 0)
    {
    }
}
