#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QHBoxLayout>
#include <QListWidget>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    mSegmentsList(0)
{
    ui->setupUi(this);

    QWidget *central = new QWidget();
    QHBoxLayout *mainLayout = new QHBoxLayout();

    mSegmentsList = new QListWidget(central);

    mainLayout->addWidget(mSegmentsList);

    central->setLayout(mainLayout);
    setCentralWidget(central);
}

MainWindow::~MainWindow()
{
    delete ui;

    delete mSegmentsList;
}
