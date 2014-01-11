#include "mainwindow.h"
#include "ui_mainwindow.h"

#include "obreader.h"
#include "molecule.h"
#include "aboutdialog.h"

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


    // default settings
    m_filePath = QDir::homePath();
    ui->molInfoDock->setVisible(true);

    initDirectoryBrowseModel();

    setWindowTitle(tr("Chemical Structure Viewer"));

    connect(ui->treeView, &QTreeView::activated, this,  &MainWindow::openFromFileIndex);
    connect(ui->actionOpenFIle, &QAction::triggered, this, &MainWindow::browseFile);
    connect(ui->actionOpenDirectory, &QAction::triggered, this, &MainWindow::browseDir);
    connect(ui->actionNewSmiString, &QAction::triggered, this, &MainWindow::newSmiStringAction);
    connect(ui->actionAbout, &QAction::triggered, this, &MainWindow::about);
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

    OBReader obReader;
    obReader.readFile(m_filePath);

    Molecule *mol = obReader.molecule();
    ui->glWidget->setMolecule(mol);
    setMolInfo(mol);
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
    QString filter;

    filter = tr("Chemical Markup Language (*.cml);;SMILES (*.smi);;"
                "MDL MOL (*.mol);;All files (*)");

    QString path =
            QFileDialog::getOpenFileName(this, tr("Open File"), m_filePath, filter);

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
    model->setNameFilters(QStringList());
    model->setNameFilterDisables(true);

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
    OBReader obReader;
    obReader.readSmiString(string);

    Molecule *mol = obReader.molecule();
    ui->glWidget->setMolecule(mol);
    setMolInfo(mol);
}

void MainWindow::about()
{
    AboutDialog dlg(this);
    dlg.exec();
}

inline void MainWindow::initDirectoryBrowseModel()
{
    QFileSystemModel *model = new QFileSystemModel(this);
    model->setRootPath(m_filePath);
    model->setReadOnly(true);

    ui->treeView->setModel(model);
    ui->treeView->setRootIndex(model->index(m_filePath));
    // need only to show name column.
    ui->treeView->hideColumn(1);
    ui->treeView->hideColumn(2);
    ui->treeView->hideColumn(3);
}

void MainWindow::setMolInfo(Molecule *mol)
{
    if (!mol)
    {
        ui->molTitle->setText("");
        ui->molFormula->setText("");
        ui->molWeight->setText("");

        return;
    }

    ui->molTitle->setText(mol->title());
    ui->molFormula->setText(mol->formula());
    ui->molWeight->setText(QString::number(mol->weight()));
}
