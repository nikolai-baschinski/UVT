#include "test.h"
#include "ui_test.h"
#include "mainwindow.h"
#include <QRandomGenerator>
#include <QVBoxLayout>
#include <QTimer>
#include <QMessageBox>

Test::Test(Test_Settings& settings, Lesson* pLesson, MainWindow* pParent)
    : QDialog(pParent)
    , ui(new Ui::Test)
{
    ui->setupUi(this);
    this->timer = new QTimer(this);
    this->delay = settings.delay;

    connect(ui->pushButton_StopTest, &QPushButton::clicked, this, &Test::pushButton_StopTest);
    connect(ui->pushButton_GoOn, &QPushButton::clicked, this, &Test::pushButton_GoOn);
    connect(this->timer, &QTimer::timeout, this, &Test::onTimer);

    this->pMainWindow = pParent;

     // Generate the test
    for(int i = 0; i < pLesson->words.count(); i++) {
        for(int j = 0; j < settings.numberCycles; j++) {
            this->words.append(pLesson->words.at(i));
        }
    }

    this->valueWordsToTest = this->words.count();
    this->ui->label_Status->setText(tr("Tested: ") + QString::number(this->valueWordsToTest - this->words.count()) + tr(" out of ") + QString::number(this->valueWordsToTest));
    this->ui->lineEdit_ForeignLang->installEventFilter(this);
    this->ui->textEdit_NativeLang->installEventFilter(this);
    this->ui->label_ForeignLang->setText(this->pMainWindow->getForeignString());
    this->ui->label_NativeLang->setText(this->pMainWindow->getNativeString());
    QFileInfo fileInfo (pLesson->path);
    this->setWindowTitle(tr("Test of the lesson ") + fileInfo.fileName());
    this->questionLangSetting = settings.language;

    if(this->questionLangSetting == Mixed) {
        int rand = QRandomGenerator::global()->bounded(2);
        if(rand == 0) {
            this->questionLangCurrent = Language::Foreign;
        } else {
            this->questionLangCurrent = Language::Native;
        }
    } else if(this->questionLangSetting == Language::Foreign) {
        this->questionLangCurrent = Language::Foreign;
    } else {
        this->questionLangCurrent = Language::Native;
    }

    this->displayWord();
}

Test::~Test()
{
    this->pMainWindow->showSelectedLesson();
    delete ui;
}

void Test::displayWord()
{
    if(this->questionLangSetting == Mixed) {
        int rand = QRandomGenerator::global()->bounded(2);
        if(rand == 0) {
            this->questionLangCurrent = Language::Foreign;
        } else {
            this->questionLangCurrent = Language::Native;
        }
    } else if(this->questionLangSetting == Language::Foreign) {
        this->questionLangCurrent = Language::Foreign;
    } else {
        this->questionLangCurrent = Language::Native;
    }

    this->currentWordIndex = QRandomGenerator::global()->bounded(this->words.count());
    this->ui->lineEdit_ForeignLang->setStyleSheet("QLineEdit { color: black; }");
    this->ui->textEdit_NativeLang->setTextColor(Qt::black);

    if(this->questionLangCurrent == Language::Native) {
        // Asking for the native word
        this->ui->lineEdit_ForeignLang->setText(this->words.at(this->currentWordIndex).foreign);
        this->ui->lineEdit_ForeignLang->setReadOnly(true);

        this->ui->textEdit_NativeLang->clear();
        this->ui->textEdit_NativeLang->setReadOnly(false);
        this->ui->textEdit_NativeLang->setFocus();

        qsizetype count = this->words.at(this->currentWordIndex).natives.count();
        this->ui->label_NumberTranslations->setText("(" + QString::number(count) + ")");

    } else {
        // Asking for the foreign word
        this->ui->textEdit_NativeLang->clear();
        for(int i = 0; i < this->words.at(this->currentWordIndex).natives.count(); i++) {
            this->ui->textEdit_NativeLang->append(this->words.at(this->currentWordIndex).natives.at(i).native);
        }
        this->ui->textEdit_NativeLang->setReadOnly(true);

        this->ui->lineEdit_ForeignLang->clear();
        this->ui->lineEdit_ForeignLang->setReadOnly(false);
        this->ui->lineEdit_ForeignLang->setFocus();

        this->ui->label_NumberTranslations->setText("");
    }
}

void Test::pushButton_StopTest()
{
    this->pMainWindow->showSelectedLesson();
    this->close();
}

void Test::pushButton_GoOn()
{
    this->ui->pushButton_GoOn->setDefault(false);
    QTextCursor cursor(this->ui->textEdit_NativeLang->textCursor());
    QTextCharFormat redFormat;
    QTextCharFormat greenFormat;
    QString foreignWordByUser = this->ui->lineEdit_ForeignLang->text().trimmed();
    Word currentWord = this->words.at(this->currentWordIndex); // needed for search

    if(this->questionLangCurrent == Language::Native) {
        QString translations = this->ui->textEdit_NativeLang->toPlainText();
        QStringList translationsList = translations.split('\n', Qt::SkipEmptyParts);

        this->ui->textEdit_NativeLang->clear();

        unsigned int numberOfTranslations = this->words.at(this->currentWordIndex).natives.count();
        unsigned int correctCounter = numberOfTranslations;

        for(int i = 0; i < numberOfTranslations; i++) {
            QString correntTranslation = this->words.at(this->currentWordIndex).natives.at(i).native;
            QString userInputTranslation;
            unsigned int tempCorrectCounter = correctCounter;
            for(int j = 0; j < translationsList.count(); j++) {
                userInputTranslation = translationsList.at(j).trimmed();
                if(correntTranslation == userInputTranslation) {       
                    correctCounter--;
                    greenFormat.setForeground(Qt::darkGreen);
                    cursor.insertText(correntTranslation + '\n', greenFormat);
                    break;
                }
            }

            if(tempCorrectCounter == correctCounter) {
                redFormat.setForeground(Qt::red);
                cursor.insertText(correntTranslation + '\n', redFormat);
            }
        }

        if(correctCounter == 0) {
            this->words.removeAt(this->currentWordIndex);
        }
    } else {
        if(foreignWordByUser == this->words.at(this->currentWordIndex).foreign) {
            // correct
            this->ui->lineEdit_ForeignLang->setStyleSheet("QLineEdit { color: green; }");
            this->words.removeAt(this->currentWordIndex);
        } else {
            this->ui->lineEdit_ForeignLang->setStyleSheet("QLineEdit { color: red; }");
            this->ui->lineEdit_ForeignLang->setText(this->words.at(this->currentWordIndex).foreign);
        }
    }

    // Look for the word that was confound
    auto lessons = pMainWindow->getLessons();
    const Lesson* pLesson;
    for(unsigned int i = 0; i < lessons.count(); i++) {
        pLesson = lessons.at(i);
        for(unsigned int j = 0; j < pLesson->words.count(); j++) {
            const Word& word = pLesson->words.at(j);
            if(word.foreign != currentWord.foreign) {
                if(word.foreign == foreignWordByUser) {
                    QString info = pLesson->path + '\n' + word.foreign + " - ";
                    for(int n = 0; n < word.natives.count(); n++) {
                        info.append(word.natives.at(n).native);
                        if(n < word.natives.count()-1) {
                            info.append(", ");
                        }
                    }
                    QMessageBox::information(this, tr("Info"), info);
                }
            }
        }
    }


    this->timer->start(this->delay);
}

void Test::onTimer()
{
    this->timer->stop();

    this->ui->label_Status->setText(tr("Tested: ") + QString::number(this->valueWordsToTest - this->words.count()) + tr(" out of ") + QString::number(this->valueWordsToTest));

    if(this->words.count() == 0) {
        QMessageBox::information(this, tr("Info"), tr("Test finished successfully."));
        this->close();
    } else {
        this->displayWord();
    }
}

bool Test::eventFilter(QObject *obj, QEvent *event)
{
    if ((obj == this->ui->lineEdit_ForeignLang || obj == this->ui->textEdit_NativeLang) && event->type() == QEvent::KeyPress) {
        QKeyEvent *keyEvent = static_cast<QKeyEvent *>(event);
        if(keyEvent->key() == Qt::Key_Tab) {
            this->ui->pushButton_GoOn->setDefault(true);
            this->ui->pushButton_GoOn->setFocus();
            return true;
        }
    }
    return QObject::eventFilter(obj, event);
}
