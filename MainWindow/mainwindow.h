#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

namespace Ui {
class MainWindow;
}

class Molecule;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

public slots:
    void openFile(const QString &path);
    void openFromFileIndex(const QModelIndex &index);
    void browseFile();
    void browseDir();
    void newSmiStringAction();
    void smiStringChanged(const QString &string);
    void about();

private:
    void initDirectoryBrowseModel();
    void setMolInfo(Molecule *mol);

    Ui::MainWindow *ui;

    QString m_filePath;
};

#endif // MAINWINDOW_H
