#include "mainwindow.h"
#include "ui_mainwindow.h"

#include "cmlreader.h"
#include "obreader.h"
#include "settingsdialog.h"

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
    m_fileReader = FileReader::ObReader;
    ui->molInfoDock->setVisible(false);

    initDirectoryBrowseModel();

    setWindowTitle(tr("Chemical Structure Viewer"));

    connect(ui->treeView, &QTreeView::activated, this,  &MainWindow::openFromFileIndex);
    connect(ui->actionOpenFIle, &QAction::triggered, this, &MainWindow::browseFile);
    connect(ui->actionOpenDirectory, &QAction::triggered, this, &MainWindow::browseDir);
    connect(ui->actionNewSmiString, &QAction::triggered, this, &MainWindow::newSmiStringAction);
    connect(ui->actionSettings, &QAction::triggered, this, &MainWindow::openSettingsDialog);
    connect(ui->smiEdit, &QLineEdit::textChanged, this, &MainWindow::smiStringChanged);
    connect(this, &MainWindow::useOpenBabel, this, &MainWindow::setUseOpenBabel);

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

    if (m_fileReader == FileReader::CmlReader)
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
    QString filter;

    if (m_fileReader == FileReader::ObReader)
    {
        filter = tr("Chemical Markup Language (*.cml);;SMILES (*.smi);;"
                    "MDL MOL (*.mol);;All files (*)");
    }
    else
    {
        filter = tr("Chemical Markup Language (*.cml);;All files (*)");
    }

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

    if (m_fileReader == FileReader::CmlReader)
    {
        model->setNameFilters(QStringList("*.cml"));
        model->setNameFilterDisables(false);
    }
    else
    {
        model->setNameFilters(QStringList());
        model->setNameFilterDisables(true);
    }

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
    if (m_fileReader != FileReader::ObReader)
        return;

    OBReader obReader;
    obReader.readSmiString(string);
    ui->glWidget->setMolecule(obReader.molecule());
}

void MainWindow::openSettingsDialog()
{
    bool usingOpenBabel = (m_fileReader == FileReader::ObReader);

    SettingsDialog dialog(usingOpenBabel, this);

    if (dialog.exec() == QDialog::Rejected)
    {
        return;
    }

    emit useOpenBabel(dialog.isOpenBabelReaderChecked());
}

void MainWindow::setUseOpenBabel(bool useOpenBabel)
{
    m_fileReader = useOpenBabel ? FileReader::ObReader : FileReader::CmlReader;

    ui->actionNewSmiString->setEnabled(useOpenBabel);
    ui->actionSmilesDock->setEnabled(useOpenBabel);
    ui->smiEdit->setEnabled(useOpenBabel);
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
