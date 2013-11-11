#include "mainwindow.h"
#include "ui_mainwindow.h"

#include "cmlreader.h"

#include <QFile>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    QFile file("../structures/drugs/4-acetamidophenyl_2-hydroxybenzoate.cml");
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        qDebug() << file.errorString();
    }
    CmlReader cmlReader(&file);
    cmlReader.parse();
    ui->glWidget->setMolecule(cmlReader.molecule());
}

MainWindow::~MainWindow()
{
    delete ui;
}
