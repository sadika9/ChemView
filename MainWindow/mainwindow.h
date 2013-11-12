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
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

public slots:
    void openFile(const QModelIndex &index);
    void browseDir();

private:
    Ui::MainWindow *ui;

    QString m_filePath;
};

#endif // MAINWINDOW_H
