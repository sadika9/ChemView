#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    enum class FileReader {CmlReader, ObReader};

    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

public slots:
    void openFile(const QString &path);
    void openFromFileIndex(const QModelIndex &index);
    void browseFile();
    void browseDir();
    void newSmiStringAction();
    void smiStringChanged(const QString &string);
    void openSettingsDialog();
    void setUseOpenBabel(bool useOpenBabel);

signals:
    void useOpenBabel(bool);

private:
    void initDirectoryBrowseModel();

    Ui::MainWindow *ui;

    FileReader m_fileReader;

    QString m_filePath;
};

#endif // MAINWINDOW_H
