#include "mainwindow.h"
#include "ui_mainwindow.h"

#include "cmlreader.h"

#include <QFileDialog>
#include <QFileSystemModel>
#include <QPushButton>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    m_filePath = QDir::currentPath();

    QFileSystemModel *model = new QFileSystemModel(this);
    model->setRootPath(m_filePath);
    model->setReadOnly(true);
    model->setNameFilters(QStringList("*.cml"));
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
    connect(ui->treeView, &QTreeView::activated, this, &MainWindow::openFile);

    // dont need these at this moment.
    ui->mainToolBar->hide();
    ui->menuBar->hide();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::openFile(const QModelIndex &index)
{
    QFileSystemModel *model = (QFileSystemModel *)ui->treeView->model();
    m_filePath = model->filePath(index);

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
