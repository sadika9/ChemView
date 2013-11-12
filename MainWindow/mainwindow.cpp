#include "mainwindow.h"
#include "ui_mainwindow.h"

#include "cmlreader.h"

#include <QFileDialog>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    connect(ui->action_Open, &QAction::triggered, this, &MainWindow::openFile);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::openFile()
{
    m_filePath = QFileDialog::getOpenFileName(this,
                                              tr("Open CML File"),
                                              m_filePath,
                                              tr("CML Files (*.cml)"));

    QFile file(m_filePath);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        qDebug() << file.errorString();
    }
    CmlReader cmlReader(&file);
    cmlReader.parse();
    ui->glWidget->setMolecule(cmlReader.molecule());
}
