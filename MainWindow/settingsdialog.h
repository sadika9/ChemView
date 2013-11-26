#ifndef SETTINGSDIALOG_H
#define SETTINGSDIALOG_H

#include <QDialog>

namespace Ui {
class SettingsDialog;
}

class SettingsDialog : public QDialog
{
    Q_OBJECT

public:
    explicit SettingsDialog(bool usingOpenBabel, QWidget *parent = 0);
    ~SettingsDialog();

    bool isOpenBabelReaderChecked();

private:
    Ui::SettingsDialog *ui;
};

#endif // SETTINGSDIALOG_H
