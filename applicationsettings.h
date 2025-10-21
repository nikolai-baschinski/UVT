#ifndef APPLICATIONSETTINGS_H
#define APPLICATIONSETTINGS_H

#include <QDialog>
#include <QString>
#include "mainwindow.h"

namespace Ui {
class ApplicationSettings;
}

class ApplicationSettings : public QDialog
{
    Q_OBJECT

public:
    explicit ApplicationSettings(MainWindow* pParent);
    ~ApplicationSettings();

    QString getForeignString();
    QString getNativeString();
    bool changeApplicationLanguage();
    QString getCurrentLanguage();
private:
    Ui::ApplicationSettings *ui;
    void onComboBoxTextChanged();
    QString currentLanguage;
    void onSpinBoxValueChanged(int newValue);
};

#endif // APPLICATIONSETTINGS_H
