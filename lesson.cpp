#include "lesson.h"
#include <QFile>
#include <QTextStream>
#include <QDebug>

Lesson::Lesson(QString absoluteFilePath) {
    QFile file(absoluteFilePath);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        return;
    }

    this->path = absoluteFilePath;

    int cntr = 0;
    QTextStream in(&file);
    while (!in.atEnd()) {
        QString line = in.readLine();
        cntr++;
        if(cntr > 2 && !line.isEmpty()) {

            Word word;
            QStringList parts = line.split("\t", Qt::SkipEmptyParts);
            UVT::Native locNative;
            switch(parts.length()) {
                case 1:
                    locNative.native = parts[0];
                    if(this->words.count() > 0) {
                        this->words.last().natives.append(locNative);
                    }
                    break;
                case 2:
                    if(line.at(0) == '\t') {
                        locNative.native = parts[0];
                        locNative.example = parts[1];
                        if(this->words.count() > 0) {
                            this->words.last().natives.append(locNative);
                        }
                    } else {
                        word.foreign = parts[0];
                        locNative.native = parts[1];
                        word.natives.append(locNative);
                        this->words.append(word);
                    }
                    break;
                case 3:
                    word.foreign = parts.at(0);
                    locNative.native = parts[1];
                    locNative.example = parts[2];
                    word.natives.append(locNative);
                    this->words.append(word);
                    break;
                default:
                    qDebug() << line;
                    break;
            }
        }
    }

    file.close();
}
