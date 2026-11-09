#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include "scanner.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    connect(ui->start, &QPushButton::clicked, this, &MainWindow::start);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::start()
{
    ui->output->setPlainText(QString::fromStdString(getLocalIP()) + QString::fromStdString(findIfaceAndNetmask(ip)));
}
