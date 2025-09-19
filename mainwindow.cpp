#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include <QMessageBox>
#include <QFileInfoList>
#include <QStandardItem>
#include <QDebug>
#include <QVBoxLayout>
#include <QKeyEvent>
#include <QFileDialog>
#include "test_configuration.h"
#include "applicationsettings.h"

MainWindow::MainWindow(QLocale paramApplicationLocale, QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    connect(ui->listWidget, &QListWidget::itemSelectionChanged, this, &MainWindow::listWidget_itemSelectionChanged);
    connect(ui->pushButton_CreateNewLesson, &QPushButton::clicked, this, &MainWindow::pushButton_CreateNewLesson);
    connect(ui->pushButton_AddFolder, &QPushButton::clicked, this, &MainWindow::pushButton_AddFolder);
    connect(ui->pushButton_UpdateLessensView, &QPushButton::clicked, this, &MainWindow::pushButton_UpdateLessensView);
    connect(ui->pushButton_TestLesson, &QPushButton::clicked, this, &MainWindow::pushButton_TestLesson);
    connect(ui->pushButton_Save, &QPushButton::clicked, this, &MainWindow::pushButton_Save);
    connect(ui->pushButton_AddRowBelow, &QPushButton::clicked, this, &MainWindow::pushButton_AddRowBelow);
    connect(ui->pushButton_InsertRowBelow, &QPushButton::clicked, this, &MainWindow::pushButton_InsertRowBelow);
    connect(ui->pushButton_InsertRowAbove, &QPushButton::clicked, this, &MainWindow::pushButton_InsertRowAbove);
    connect(ui->pushButton_RemoveRow, &QPushButton::clicked, this, &MainWindow::pushButton_RemoveRow);
    connect(ui->pushButton_Search, &QPushButton::clicked, this, &MainWindow::pushButton_Search);
    connect(ui->lineEdit_SearchString, &QLineEdit::returnPressed, this, &MainWindow::pushButton_Search);
    connect(ui->pushButton_ApplicationSettings, &QPushButton::clicked, this, &MainWindow::pushButton_ApplicationSettings);

    this->ui->listWidget->installEventFilter(this);
    this->ui->lineEdit_SearchString->installEventFilter(this);
    this->applicationLocale = paramApplicationLocale;

    this->settings = new QSettings("settings.ini", QSettings::IniFormat);
    QString foldersString = this->settings->value("Folders").toString();
    this->foreignString = this->settings->value("ForeignLabel").toString();
    if(this->foreignString.isEmpty()) this->foreignString = "English";
    this->nativeString = this->settings->value("NativeLabel").toString();
    if(this->nativeString.isEmpty()) this->nativeString = "German";

    // Layout
    // TODO

    ui->listWidget->clear();

    QStringList folders = foldersString.split('\n', Qt::SkipEmptyParts);
    for (int i = 0; i < folders.count(); i++) {
        QDir dir(folders.at(i));
        if (dir.exists()) {
            this->loadLessons(dir);
            this->ui->plainTextEdit_Folders->appendPlainText(dir.path());
        }
    }

    ui->tableWidget->clear();
    ui->tableWidget->setColumnCount(3);
    ui->tableWidget->horizontalHeader()->setStretchLastSection(true);
    ui->tableWidget->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->tableWidget->setHorizontalHeaderLabels({this->foreignString, this->nativeString, tr("Example")});
    ui->tableWidget->setEditTriggers(QAbstractItemView::DoubleClicked | QAbstractItemView::SelectedClicked);
    ui->tableWidget->horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);
    QFont font = this->ui->tableWidget->horizontalHeader()->font();
    font.setBold(true);
    this->ui->tableWidget->horizontalHeader()->setFont(font);

    QString lastLesson = this->settings->value("LastLesson").toString();
    for (int i = 0; i < this->lessons.count(); i++) {
        if (this->lessons.at(i)->path == lastLesson) {
            ui->listWidget->setCurrentRow(i);
            break;
        }
    }

    if (ui->listWidget->count() > 0 && ui->listWidget->currentRow() == -1) {
        ui->listWidget->setCurrentRow(0);
    }

    ui->tableWidget->resizeColumnsToContents();
    ui->listWidget->setFocus();
    ui->plainTextEdit_SearchResults->setReadOnly(true);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::pushButton_CreateNewLesson()
{
    QString path = settings->value("NewLessonSavePath").toString();
    QString fileName = QFileDialog::getSaveFileName(this, tr("Create new lessson"), path, tr("Text Files (*.txt);;All Files (*)"), nullptr, QFileDialog::DontConfirmOverwrite);
    if (!fileName.isEmpty()) {
        // check whether the lesson name already exists
        int lessonExistsIndex = -1;
        for(int i = 0; i < this->lessons.count(); i++) {
            if (QDir::cleanPath(this->lessons.at(i)->path) == QDir::cleanPath(fileName)) {
                lessonExistsIndex = i;
                break;
            }
        }
        if(lessonExistsIndex != -1) {
            QMessageBox::information(this, tr("Info"), tr("Lesson with this name exists already. Please choose another name."));
            ui->listWidget->setCurrentRow(lessonExistsIndex);
        } else {
            QFile file(fileName);
            if (file.open(QIODevice::WriteOnly | QIODevice::Text)) {
                QFileInfo fileInfo(fileName);
                QString lessonName = fileInfo.completeBaseName();
                QTextStream out(&file);
                out << lessonName << "\n";
                file.close();

                QListWidgetItem *item = new QListWidgetItem(lessonName, ui->listWidget);
                item->setToolTip(fileInfo.absolutePath());
                Lesson *lesson = new Lesson(fileName);
                this->lessons.append(lesson);
                this->ui->listWidget->setCurrentRow(this->ui->listWidget->count() - 1);
                settings->setValue("NewLessonSavePath", fileInfo.absolutePath());
            } else {
                QMessageBox::warning(this, tr("Error"), tr("Could not create file: ") + file.errorString());
            }
        }
    }
}

void MainWindow::fillTable(Lesson* pLesson)
{
    disconnect(ui->tableWidget, &QTableWidget::cellChanged, this, &MainWindow::onCellContentChanged);

    ui->tableWidget->clearContents();   // delete only the content of the table, the header remains
    ui->tableWidget->setRowCount(0);    // remove all rows of the table
    int rowIndex = 0;

    if(pLesson == nullptr) {
        QMessageBox::information(this, tr("Error"), tr("Internal error occured. Lesson pointer 0."));
        return;
    }
    for (int i = 0; i < pLesson->words.size(); i++) {
        const Word &word = pLesson->words.at(i);

        for (int n = 0; n < word.natives.size(); n++) {
            ui->tableWidget->insertRow(rowIndex);

            // Column 0: Word
            if (n == 0) {
                ui->tableWidget->setItem(rowIndex, 0, new QTableWidgetItem(word.foreign));
            } else {
                ui->tableWidget->setItem(rowIndex, 0, new QTableWidgetItem(""));
            }

            // Column 1: Translation
            ui->tableWidget->setItem(rowIndex, 1, new QTableWidgetItem(word.natives.at(n).native));

            // Column 2: Note
            ui->tableWidget->setItem(rowIndex, 2, new QTableWidgetItem(word.natives.at(n).example));

            rowIndex++;
        }
    }

    connect(ui->tableWidget, &QTableWidget::cellChanged, this, &MainWindow::onCellContentChanged);
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    QListWidgetItem *currentItem = ui->listWidget->currentItem();
    if (currentItem) {
        settings->setValue("LastLesson", currentItem->toolTip() + "/" + currentItem->text() + ".txt");
    }

    settings->setValue("Folders", this->ui->plainTextEdit_Folders->toPlainText());

    QMainWindow::closeEvent(event);
}

bool MainWindow::eventFilter(QObject *obj, QEvent *event)
{
    if (obj == this->ui->listWidget && event->type() == QEvent::KeyPress) {
        QKeyEvent *keyEvent = static_cast<QKeyEvent *>(event);
        int currentRow = this->ui->listWidget->currentRow();
        if(keyEvent->key() == Qt::Key_Up) {
            if(currentRow > 0) {
                this->ui->listWidget->setCurrentRow(currentRow - 1);
            }
            return true;
        }

        if(keyEvent->key() == Qt::Key_Down) {
            if(currentRow < this->ui->listWidget->count() - 1) {
                this->ui->listWidget->setCurrentRow(currentRow + 1);
            }
            return true;
        }
    }

    if ((obj == this->ui->lineEdit_SearchString) && event->type() == QEvent::KeyPress) {
        QKeyEvent *keyEvent = static_cast<QKeyEvent *>(event);
        if(keyEvent->key() == Qt::Key_Tab) {
            this->ui->pushButton_Search->setDefault(true);
            this->ui->pushButton_Search->setFocus();
            return true;
        }
    }

    return QMainWindow::eventFilter(obj, event);
}

void MainWindow::listWidget_itemSelectionChanged()
{
    int row = this->ui->listWidget->currentRow();
    if(this->lessons.count() > 0 && row >= 0) {
        this->fillTable(this->lessons.at(row));
    }

    // add 10 new rows to new lessons
    int rowCnt = this->ui->tableWidget->rowCount();
    if(rowCnt == 0) {
        ui->tableWidget->setRowCount(10);
        for (int i = 0; i < 10; ++i) {
            ui->tableWidget->setItem(i, 0, new QTableWidgetItem(""));
            ui->tableWidget->setItem(i, 1, new QTableWidgetItem(""));
            ui->tableWidget->setItem(i, 2, new QTableWidgetItem(""));
        }
    }
}

void MainWindow::pushButton_AddFolder()
{
    QString folder = QFileDialog::getExistingDirectory(this, tr("Choose a folder please."), QDir::currentPath(), QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);
    if (!folder.isEmpty()) {

        bool folderAlreadyIncluded = false;
        QString currentFolders = this->ui->plainTextEdit_Folders->toPlainText();

        QStringList folders = currentFolders.split('\n', Qt::SkipEmptyParts);
        for (int i = 0; i < folders.count(); i++) {
            if(folders.at(i).trimmed() == folder.trimmed()) {
                folderAlreadyIncluded = true;
                break;
            }
        }

        if(folderAlreadyIncluded) {
            QMessageBox::information(this, tr("Info"), tr("Folder already included."));
        } else {
            QDir dir(folder);
            this->loadLessons(dir);
            this->ui->plainTextEdit_Folders->appendPlainText(folder);

            if (ui->listWidget->count() > 0 && ui->listWidget->currentRow() == -1) {
                ui->listWidget->setCurrentRow(0);
            }
        }
    }
}

void MainWindow::loadLessons(QDir dir)
{
    const QFileInfoList list = dir.entryInfoList(QStringList() << "*.txt", QDir::Files | QDir::NoSymLinks);
    for (const QFileInfo& fileInfo : list) {
        QListWidgetItem* item = new QListWidgetItem(fileInfo.completeBaseName(), ui->listWidget);
        item->setToolTip(fileInfo.absolutePath());  // Tooltip: only the path

        Lesson *lesson = new Lesson(fileInfo.absoluteFilePath());
        this->lessons.append(lesson);
    }
}

void MainWindow::pushButton_UpdateLessensView()
{
    QString memorySelectedLesson;
    if (auto *item = ui->listWidget->currentItem()) {
        memorySelectedLesson = item->text();
    }

    qDeleteAll(this->lessons);
    this->lessons.clear();
    this->ui->listWidget->clear();

    QString foldersString = ui->plainTextEdit_Folders->toPlainText();
    QStringList folders = foldersString.split('\n', Qt::SkipEmptyParts);
    for (int i = 0; i < folders.count(); i++) {
        QDir dir(folders.at(i).trimmed());
        if (dir.exists()) {
           this->loadLessons(dir);
        }
    }

    bool found = false;
    for(int i = 0; i < this->ui->listWidget->count(); i++) {
        if(this->ui->listWidget->item(i)->text() == memorySelectedLesson) {
            this->ui->listWidget->setCurrentRow(i);
            found = true;
            break;
        }
    }

    if (this->ui->listWidget->count() > 0) {
        if(found == false) {
            this->ui->listWidget->setCurrentRow(0);
        }
    } else {
        ui->tableWidget->clearContents();
        ui->tableWidget->setRowCount(0);
    }
}

void MainWindow::pushButton_TestLesson()
{
    int row = this->ui->listWidget->currentRow();
    if(row >= 0) {
        Test_Configuration tstConfig(this->lessons[row], this);
        tstConfig.exec();
    } else {
        QMessageBox::information(this, tr("Info"), tr("Please select a lesson for test."));
    }
}

void MainWindow::pushButton_Save()
{
    // Input check
    unsigned int numbRows = this->ui->tableWidget->rowCount();
    QString text_nx_c0;
    QString text_nx_c1;
    QString text_nx_c2;
    for(int i = 0; i < numbRows; i++) {
        text_nx_c0 = this->ui->tableWidget->item(i, 0)->text();
        text_nx_c1 = this->ui->tableWidget->item(i, 1)->text();
        text_nx_c2 = this->ui->tableWidget->item(i, 2)->text();
        if(text_nx_c0.isEmpty() && text_nx_c1.isEmpty() && !text_nx_c2.isEmpty()) {
            QMessageBox::information(this, tr("Info"), tr("Please check content in line ") + QString::number(i+1));
            return;
        }

        if(!text_nx_c0.isEmpty() && text_nx_c1.isEmpty()) {
            QMessageBox::information(this, tr("Info"), tr("Please check content in line ") + QString::number(i+1));
            return;
        }

        if(text_nx_c0.isEmpty() && !text_nx_c1.isEmpty()) {
            int counter = i;
            QString text_rx_c0;
            QString text_rx_c1;
            while(counter > 0) {
                text_rx_c0 = this->ui->tableWidget->item(counter-1, 0)->text();
                text_rx_c1 = this->ui->tableWidget->item(counter-1, 1)->text();
                if(text_rx_c0.isEmpty() && text_rx_c1.isEmpty()) {
                    QMessageBox::information(this, tr("Info"), tr("Please check content in line ") + QString::number(i+1));
                    return;
                }
                if(!text_rx_c0.isEmpty() && !text_rx_c1.isEmpty()) {
                    break;
                }
                if(text_rx_c0.isEmpty() && !text_rx_c1.isEmpty()) {
                    counter--;
                }
            }

            if(counter == 0) {
                QMessageBox::information(this, tr("Info"), tr("Please check content in line ") + QString::number(i+1));
                return;
            }
        }
    }

    QListWidgetItem* pListItem = this->ui->listWidget->currentItem();
    if(pListItem == nullptr) {
        return;
    }
    QString lessonName = pListItem->text();
    QString fileName = pListItem->toolTip() + "/" + lessonName + ".txt";
    QFile file(fileName);
    if (file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        QTextStream out(&file);
        out << lessonName << "\n\n";

        unsigned int longest_C0 = 0;
        unsigned int longest_C1 = 0;

        // Search for the longest word
        for(int i = 0; i < numbRows; i++) {
            text_nx_c0 = this->ui->tableWidget->item(i, 0)->text();
            if(text_nx_c0.length() > longest_C0) {
                longest_C0 = text_nx_c0.length();
            }
            text_nx_c1 = this->ui->tableWidget->item(i, 1)->text();
            if(text_nx_c1.length() > longest_C1) {
                longest_C1 = text_nx_c1.length();
            }
        }

        unsigned int tabs_C0 = longest_C0/4+1;
        unsigned int tabs_C1 = longest_C1/4+1;

        unsigned int tab_loops = 0;
        for(int i = 0; i < numbRows; i++) {
            text_nx_c0 = this->ui->tableWidget->item(i, 0)->text();
            text_nx_c1 = this->ui->tableWidget->item(i, 1)->text();
            text_nx_c2 = this->ui->tableWidget->item(i, 2)->text();

            // Skip empty rows
            if(text_nx_c0.isEmpty() && text_nx_c1.isEmpty() && text_nx_c2.isEmpty()) {
                continue;
            }

            out << text_nx_c0;
            tab_loops = tabs_C0 - text_nx_c0.length() / 4;
            for(int j = 0; j < tab_loops; j ++) {
                out << '\t';
            }

            out << text_nx_c1;
            tab_loops = tabs_C1 - text_nx_c1.length() / 4;
            for(int j = 0; j < tab_loops; j ++) {
                out << '\t';
            }

            out << this->ui->tableWidget->item(i, 2)->text() << '\n';
        }
        file.close();

        int row = this->ui->listWidget->currentRow();
        if(this->lessons[row] != nullptr) {
            delete this->lessons[row];
            Lesson* pNewLesson = new Lesson(fileName);
            this->lessons[row] = pNewLesson;
            this->fillTable(this->lessons.at(row));
        }

    } else {
        QMessageBox::information(this, tr("Info"), "An error occured while saving.");
    }
}

void MainWindow::pushButton_AddRowBelow()
{
    int row = ui->tableWidget->rowCount();
    ui->tableWidget->insertRow(row);
    ui->tableWidget->setItem(row, 0, new QTableWidgetItem(""));
    ui->tableWidget->setItem(row, 1, new QTableWidgetItem(""));
    ui->tableWidget->setItem(row, 2, new QTableWidgetItem(""));
}

void MainWindow::pushButton_InsertRowBelow()
{
    int row = ui->tableWidget->currentRow();
    if(row != -1) {
        ui->tableWidget->insertRow(row+1);
        ui->tableWidget->setItem(row+1, 0, new QTableWidgetItem(""));
        ui->tableWidget->setItem(row+1, 1, new QTableWidgetItem(""));
        ui->tableWidget->setItem(row+1, 2, new QTableWidgetItem(""));
    }
}

void MainWindow::pushButton_InsertRowAbove()
{
    int row = ui->tableWidget->currentRow();
    if(row != -1) {
        ui->tableWidget->insertRow(row);
        ui->tableWidget->setItem(row, 0, new QTableWidgetItem(""));
        ui->tableWidget->setItem(row, 1, new QTableWidgetItem(""));
        ui->tableWidget->setItem(row, 2, new QTableWidgetItem(""));
    }
}

void MainWindow::pushButton_RemoveRow()
{
    this->ui->tableWidget->removeRow(ui->tableWidget->currentRow());
}

void MainWindow::hideSelectedLesson()
{
    ui->tableWidget->clearContents();
    ui->tableWidget->setRowCount(0);
}

void MainWindow::showSelectedLesson()
{
    this->listWidget_itemSelectionChanged();
}

void MainWindow::pushButton_Search()
{
    this->ui->plainTextEdit_SearchResults->clear();

    QString ss = this->ui->lineEdit_SearchString->text();
    if(ss.length() < 3 ) {
        this->ui->plainTextEdit_SearchResults->setPlainText(tr("Please enter at least three letters for the search text."));
        return;
    }

    for(int i = 0; i < this->lessons.count(); i++) {

        Lesson* pLesson = this->lessons.at(i);
        if (!pLesson) continue;
        for(int j = 0; j < pLesson->words.count(); j++) {
            const Word &word = pLesson->words.at(j);
            bool foundFlag = false;
            if(word.foreign.contains(ss)) {
                foundFlag = true;
            }
            for(int n = 0; n < word.natives.count(); n++) {
                if(word.natives.at(n).native.contains(ss)) {
                    foundFlag = true;
                }
            }
            if(foundFlag) {
                QString result = pLesson->path + '\n';
                result.append(word.foreign + '\n');
                for(int n = 0; n < word.natives.count(); n++) {
                    result.append(word.natives.at(n).native + "  " + word.natives.at(n).example + '\n');
                }
                result.append('\n');
                this->ui->plainTextEdit_SearchResults->appendPlainText(result);
            }
        }
    }
    if(this->ui->plainTextEdit_SearchResults->toPlainText() == ""){
        this->ui->plainTextEdit_SearchResults->setPlainText(tr("Nothing found"));
    }
}

void MainWindow::pushButton_ApplicationSettings()
{
    ApplicationSettings applicationSettings(this);
    auto rvDialog = applicationSettings.exec();
    if (rvDialog == QDialog::Accepted) {
        this->foreignString = applicationSettings.getForeignString();
        this->nativeString = applicationSettings.getNativeString();
        this->settings->setValue("ForeignLabel", this->foreignString);
        this->settings->setValue("NativeLabel", this->nativeString);
        ui->tableWidget->setHorizontalHeaderLabels({this->foreignString, this->nativeString});

        QString languageAppliactionSettings = applicationSettings.getCurrentLanguage();
        if (this->settings->value("ApplicationLangauge").toString() != languageAppliactionSettings)
        {
            this->settings->setValue("ApplicationLangauge", languageAppliactionSettings);
            QMessageBox::information(this, tr("Info"), tr("Please restart the application."));
        }
    }
}

QString MainWindow::getForeignString()
{
    return this->foreignString;
}

QString MainWindow::getNativeString()
{
    return this->nativeString;
}

QLocale MainWindow::getApplicationLocale()
{
    return this->applicationLocale;
}

const QVector<Lesson*>& MainWindow::getLessons() const {
    return this->lessons;
}

void MainWindow::onCellContentChanged(int row, int column)
{
    QString newValue = ui->tableWidget->item(row, column)->text();
    if(newValue.length() < 3) {
        return;
    }

    // Inform the user if the word allready exsists
    const Lesson* pLesson;
    for(unsigned int i = 0; i < lessons.count(); i++) {
        pLesson = lessons.at(i);
        for(unsigned int j = 0; j < pLesson->words.count(); j++) {
            const Word& word = pLesson->words.at(j);
            if(word.foreign == newValue) {
                QString info = tr("This word exists in lesson\n") + pLesson->path;
                QMessageBox::information(this, tr("Info"), info);
            }
        }
    }
}
