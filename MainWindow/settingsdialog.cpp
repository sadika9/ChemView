#include "settingsdialog.h"
#include "ui_settingsdialog.h"

SettingsDialog::SettingsDialog(bool usingOpenBabel,QWidget *parent) :
    QDialog(parent),
    ui(new Ui::SettingsDialog)
{
    ui->setupUi(this);

    // TODO: Need to read current settings from file or get from main window.

    ui->obReaderRadio->setChecked(usingOpenBabel);
    ui->cmlReaderRadio->setChecked(!usingOpenBabel);
}

SettingsDialog::~SettingsDialog()
{
    delete ui;
}

bool SettingsDialog::isOpenBabelReaderChecked()
{
    return ui->obReaderRadio->isChecked();
}
