#include "applicationsettings.h"
#include "ui_applicationsettings.h"
#include <QMessageBox>

ApplicationSettings::ApplicationSettings(MainWindow* pParamParent)
    : QDialog(pParamParent)
    , ui(new Ui::ApplicationSettings)
{
    ui->setupUi(this);

    this->ui->lineEdit_ForeignLanguage->setText(pParamParent->getForeignString());
    this->ui->lineEdit_NativeLanguage->setText(pParamParent->getNativeString());

    QDir dir(QDir::currentPath());
    QStringList filters;
    filters << "*.qm";

    QLocale currentApplicationLocale = pParamParent->getApplicationLocale();
    int memoryIndex = 0;

    const QStringList files = dir.entryList(filters, QDir::Files);
    for (int i = 0; i < files.count(); i++) {
        QString localeCode = files.at(i).mid(4,5);
        QLocale locale(localeCode);
        QLocale::Language lang = locale.language();
        QString langName = QLocale::languageToString(lang);
        this->ui->comboBox_ApplicationsLanguage->addItem(langName);
        if(locale == currentApplicationLocale) {
            memoryIndex = i;
            this->currentLanguage = langName;
        }
    }

    this->ui->comboBox_ApplicationsLanguage->setCurrentIndex(memoryIndex);

    connect(ui->comboBox_ApplicationsLanguage, &QComboBox::currentIndexChanged, this, &ApplicationSettings::onComboBoxTextChanged);
}

ApplicationSettings::~ApplicationSettings()
{
    delete ui;
}

QString ApplicationSettings::getForeignString()
{
    return this->ui->lineEdit_ForeignLanguage->text();
}

QString ApplicationSettings::getNativeString()
{
    return this->ui->lineEdit_NativeLanguage->text();
}

void ApplicationSettings::onComboBoxTextChanged()
{
    this->currentLanguage = this->ui->comboBox_ApplicationsLanguage->currentText();
}

QString ApplicationSettings::getCurrentLanguage()
{
    return this->currentLanguage;
}
