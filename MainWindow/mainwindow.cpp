#include "mainwindow.h"
#include "ui_mainwindow.h"

#include "cmlreader.h"
#include "obreader.h"

#include <QFileDialog>
#include <QFileSystemModel>
#include <QPushButton>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    m_filePath = QDir::currentPath();
    m_reader = FileReader::ObReader;

    QFileSystemModel *model = new QFileSystemModel(this);
    model->setRootPath(m_filePath);
    model->setReadOnly(true);
    //    model->setNameFilters(QStringList("*.cml"));
    model->setNameFilterDisables(false);

    ui->treeView->setModel(model);
    ui->treeView->setRootIndex(model->index(m_filePath));
    // need only to show name column.
    ui->treeView->hideColumn(1);
    ui->treeView->hideColumn(2);
    ui->treeView->hideColumn(3);

    ui->addressEdit->setText(m_filePath);

    setWindowTitle(tr("Chemical Structure Viewer"));
    connect(ui->browseButton, &QPushButton::clicked, this, &MainWindow::browseDir);
    connect(ui->treeView, &QTreeView::activated, this,  &MainWindow::openFromFileIndex);
    connect(ui->actionOpenFIle, &QAction::triggered, this, &MainWindow::browseFile);
    connect(ui->actionOpenDirectory, &QAction::triggered, this, &MainWindow::browseDir);

    // dont need these at this moment.
    ui->mainToolBar->hide();
//    ui->menuBar->hide();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::openFile(const QString &path)
{
    m_filePath = path;

    if (m_reader == FileReader::CmlReader)
    {
        QFile file(m_filePath);
        if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
        {
            qDebug() << file.errorString();
            return;
        }

        CmlReader cmlReader(&file);

        if (!cmlReader.parse())
        {
            qDebug() << "Invalid CML file.";
            return;
        }

        ui->glWidget->setMolecule(cmlReader.molecule());
    }
    else // ObReader
    {
        OBReader obReader;
        obReader.readFile(m_filePath);
        ui->glWidget->setMolecule(obReader.molecule());
    }
}

void MainWindow::openFromFileIndex(const QModelIndex &index)
{
    QFileSystemModel *model = (QFileSystemModel *)ui->treeView->model();

    openFile(model->filePath(index));
}

void MainWindow::browseFile()
{
    QString path = QFileDialog::getOpenFileName(this,
                                              tr("Open File"),
                                              m_filePath,
                                              tr("Chemical Markup Language (*.cml);;"
                                                 "SMILES (*.smi);;"
                                                 "MDL MOL (*.mol);;"
                                                 "All files (*)"));

    openFile(path);
}

void MainWindow::browseDir()
{
    m_filePath = QFileDialog::getExistingDirectory(this,
                                                   tr("Open Directory"),
                                                   m_filePath,
                                                   QFileDialog::ShowDirsOnly |
                                                   QFileDialog::DontResolveSymlinks);

    QFileSystemModel *model =  (QFileSystemModel *) ui->treeView->model();
    ui->treeView->setRootIndex(model->index(m_filePath));

    ui->addressEdit->setText(m_filePath);
}
