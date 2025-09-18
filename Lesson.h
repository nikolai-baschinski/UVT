#ifndef LESSON_H
#define LESSON_H
#include "word.h"
#include <QString>
#include <QList>

class Lesson
{
public:
    Lesson(QString absoluteFilePath);
    QList<Word> words;
    QString path;
};

#endif // LESSON_H
