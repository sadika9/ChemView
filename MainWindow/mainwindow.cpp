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

    ui->directoryDock->setVisible(false);
    ui->smiDock->setVisible(false);

    m_filePath = QDir::homePath();
    m_reader = FileReader::ObReader;

    initDirectoryBrowseModel();

    setWindowTitle(tr("Chemical Structure Viewer"));

    connect(ui->treeView, &QTreeView::activated, this,  &MainWindow::openFromFileIndex);
    connect(ui->actionOpenFIle, &QAction::triggered, this, &MainWindow::browseFile);
    connect(ui->actionOpenDirectory, &QAction::triggered, this, &MainWindow::browseDir);
    connect(ui->actionNewSmiString, &QAction::triggered, this, &MainWindow::newSmiStringAction);
    connect(ui->smiEdit, &QLineEdit::textChanged, this, &MainWindow::smiStringChanged);

    // dont need these at this moment.
    ui->mainToolBar->hide();
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

    if (model->isDir(index))
        return;

    openFile(model->filePath(index));
}

void MainWindow::browseFile()
{
    QString path =
            QFileDialog::getOpenFileName(this,
                                         tr("Open File"),
                                         m_filePath,
                                         tr("Chemical Markup Language (*.cml);;"
                                            "SMILES (*.smi);;"
                                            "MDL MOL (*.mol);;"
                                            "All files (*)"));
    if (path.isEmpty())
        return;

    ui->directoryDock->setVisible(false);
    ui->smiDock->setVisible(false);

    openFile(path);
}

void MainWindow::browseDir()
{
    QString path =
            QFileDialog::getExistingDirectory(this,
                                              tr("Open Directory"),
                                              m_filePath,
                                              QFileDialog::ShowDirsOnly |
                                              QFileDialog::DontResolveSymlinks);
    if (path.isEmpty())
        return;

    m_filePath = path;

    QFileSystemModel *model =  (QFileSystemModel *) ui->treeView->model();
    ui->treeView->setRootIndex(model->index(m_filePath));
    ui->directoryDock->setVisible(true);
    ui->smiDock->setVisible(false);
}

void MainWindow::newSmiStringAction()
{
    ui->directoryDock->setVisible(false);
    ui->smiDock->setVisible(true);
}

void MainWindow::smiStringChanged(const QString &string)
{
    qDebug() << "erer";
    if (m_reader != FileReader::ObReader)
        return;

    OBReader obReader;
    obReader.readSmiString(string);
    ui->glWidget->setMolecule(obReader.molecule());
}

inline void MainWindow::initDirectoryBrowseModel()
{
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
}
