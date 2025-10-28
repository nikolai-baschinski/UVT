#ifndef TEST_H
#define TEST_H

#include <QDialog>
#include <QVector>
#include <QLabel>
#include <QTextCursor>
#include "test_settings.h"
#include "lesson.h"
#include "mainwindow.h"


namespace Ui {
class Test;
}

class Test : public QDialog
{
    Q_OBJECT

public:
    explicit Test(Test_Settings& settings, const Lesson* pLesson, MainWindow* pParent = nullptr);
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
    Language questionLangCurrent;
    Language questionLangSetting;
    QTimer *timer;
    unsigned int delay;
    unsigned int valueWordsToTest;

    void displayWord();
    bool eventFilter(QObject *obj, QEvent *event);
    void showSearchResult(const Lesson* pLesson, const Word* word);
};

#endif // TEST_H
