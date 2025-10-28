#ifndef TEST_CONFIGURATION_H
#define TEST_CONFIGURATION_H

#include <QDialog>
#include <QSettings>
#include "mainwindow.h"
#include "lesson.h"

namespace Ui {
class Test_Configuration;
}

class Test_Configuration : public QDialog
{
    Q_OBJECT

public:
    explicit Test_Configuration(const Lesson* pLesson, MainWindow* pParent = nullptr);
    ~Test_Configuration();

private slots:
    void onOK();
    void onCancel();

private:
    Ui::Test_Configuration *ui;
    MainWindow* pMainWindow;
    const Lesson* pLesson;
    QSettings* settings;
};

#endif // TEST_CONFIGURATION_H
