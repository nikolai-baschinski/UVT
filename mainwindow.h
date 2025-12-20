#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QVector>
#include <QListWidgetItem>
#include <QCloseEvent>
#include <QSettings>
#include <QDir>
#include <QApplication>

#include "lesson.h"

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QLocale paramApplicationLocale, QWidget *parent = nullptr);
    ~MainWindow();
    void hideSelectedLesson();
    void showSelectedLesson();
    QString getForeignString();
    QString getNativeString();
    QLocale getApplicationLocale();
    const QVector<Lesson*>& getLessons() const;

private slots:
    void listWidget_itemSelectionChanged();
    void pushButton_CreateNewLesson();
    void pushButton_AddFolder();
    void pushButton_UpdateLessensView();
    void pushButton_TestLesson();
    void pushButton_Save();
    void pushButton_AddRowBelow();
    void pushButton_InsertRowBelow();
    void pushButton_InsertRowAbove();
    void pushButton_RemoveRow();
    void pushButton_Search();
    void pushButton_ApplicationSettings();
    void onCellContentChanged(int row, int column);
    void listWidget_SearchResults_itemSelectionChanged();

private:
    Ui::MainWindow *ui;
    void fillTable(const Lesson* pLesson);
    QVector<Lesson*> lessons;
    void closeEvent(QCloseEvent *event);
    QSettings* settings;
    bool eventFilter(QObject *obj, QEvent *event);
    void loadLessons(QDir dir);
    QString foreignString;
    QString nativeString;
    QLocale applicationLocale;
    QString checkStringMemory;
    int selectedRowMemory;
    bool checkInputCorrectness();
    void writeLessonToFile(int lessonRow);
    void checkForChanges();
    bool CtrlButtonPressed;
};
#endif // MAINWINDOW_H
