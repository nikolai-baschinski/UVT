#ifndef TEST_H
#define TEST_H

#include <QDialog>
#include "test_settings.h"
#include <QVector>
#include <QLabel>
#include "Lesson.h"
#include "mainwindow.h"
#include <QTextCursor>

namespace Ui {
class Test;
}

class Test : public QDialog
{
    Q_OBJECT

public:
    explicit Test(Test_Settings& settings, Lesson* pLesson, MainWindow* pParent = nullptr);
    ~Test();
private slots:
    void pushButton_StopTest();
    void pushButton_GoOn();
    void onTimer();

private:
    Ui::Test *ui;
    MainWindow* pMainWindow;
    QVector<Word> words;
    unsigned int currentWordIndex;
    Language questionLang;
    QTimer *timer;
    unsigned int delay;
    void displayWord();
    unsigned int valueWordsToTest;
    bool eventFilter(QObject *obj, QEvent *event);
};

#endif // TEST_H
