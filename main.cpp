#include "mainwindow.h"

#include <QApplication>
#include <QTranslator>

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    QSettings settings = QSettings("settings.ini", QSettings::IniFormat);
    QString storedApplicationLocaleString = settings.value("ApplicationLangauge").toString();

    QLocale langauageToLoad(QLocale::English, QLocale::UnitedKingdom);
    if (storedApplicationLocaleString == "") {
        QLocale locale = QLocale::system();
        if (QLocale::languageToString(locale.language()) == "German") {
            langauageToLoad = QLocale(QLocale::German, QLocale::Germany);
            settings.setValue("ApplicationLangauge", "German");
        } else {
            settings.setValue("ApplicationLangauge", "English");
        }
    } else {
        if(storedApplicationLocaleString == "German") {
            langauageToLoad = QLocale(QLocale::German, QLocale::Germany);
        }
    }

    QTranslator translator;
    if (langauageToLoad == QLocale(QLocale::German, QLocale::Germany)) {
        if (translator.load("UVT_de_DE.qm")) {
            app.installTranslator(&translator);
        }
    } else {
        if (translator.load("UVT_en_GB.qm")) {
            app.installTranslator(&translator);
        }
    }

    QString strFontSize = settings.value("ApplicationFontSize").toString();
    unsigned int intFontSize = strFontSize.toInt();
    if (intFontSize >= 9 && intFontSize <= 12) {
        QFont font = qApp->font();
        font.setPointSize(intFontSize);
        qApp->setFont(font);
    } else {
        settings.setValue("ApplicationFontSize", 9);
    }

    MainWindow w(langauageToLoad);
    w.show();
    return app.exec();
}
