#include "test_configuration.h"
#include "ui_test_configuration.h"
#include "test_settings.h"
#include "test.h"
#include "mainwindow.h"

constexpr const char* NATIVE = "Native";
constexpr const char* FOREIGN = "Foreign";

Test_Configuration::Test_Configuration(Lesson* pLesson, MainWindow* pParent)
    : QDialog(pParent)
    , ui(new Ui::Test_Configuration)
{
    ui->setupUi(this);
    connect(ui->buttonBox, &QDialogButtonBox::accepted, this, &Test_Configuration::onOK);
    connect(ui->buttonBox, &QDialogButtonBox::rejected, this, &Test_Configuration::onCancel);
    ui->lineEdit_Delay->setValidator(new QIntValidator(0, 99999, this));
    this->pMainWindow = pParent;
    this->pLesson = pLesson;

    this->settings = new QSettings("settings.ini", QSettings::IniFormat, this);
    QString testLangSelection= this->settings->value("LastTestLangSelection").toString();
    if(testLangSelection == NATIVE) {
        this->ui->radioButton_NativeLang->setChecked(true);
    } else if(testLangSelection == FOREIGN){
        this->ui->radioButton_ForeignLang->setChecked(true);
    } else {
        this->ui->radioButton_Mixed->setChecked(true);
    }

    this->ui->radioButton_ForeignLang->setText(this->pMainWindow->getForeignString());
    this->ui->radioButton_NativeLang->setText(this->pMainWindow->getNativeString());
}

Test_Configuration::~Test_Configuration()
{
    delete ui;
}

void Test_Configuration::onOK()
{
    Test_Settings testSettings;
    testSettings.delay = this->ui->lineEdit_Delay->text().toUInt();
    if(this->ui->radioButton_NativeLang->isChecked()) {
        testSettings.language = Language::Native;
        this->settings->setValue("LastTestLangSelection", NATIVE);
    } else if(this->ui->radioButton_ForeignLang->isChecked()) {
        testSettings.language = Language::Foreign;
        this->settings->setValue("LastTestLangSelection", FOREIGN);
    } else {
        testSettings.language = Language::Mixed;
        this->settings->setValue("LastTestLangSelection", "Mixed");
    }
    testSettings.numberCycles = this->ui->spinBox_Cycles->value();


    this->pMainWindow->hideSelectedLesson();

    Test test(testSettings, this->pLesson,  this->pMainWindow);
    test.exec();
}

void Test_Configuration::onCancel()
{
    this->pMainWindow->showSelectedLesson();
}
