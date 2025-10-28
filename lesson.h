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
    QString checkString;
    void updateCheckString();
};

#endif // LESSON_H
