#ifndef WORD_H
#define WORD_H

#include <QString>
#include <QList>
#include "native.h"

class Word
{
public:
    Word();
    QString foreign;
    QList<UVT::Native> natives;
    unsigned int id;
};

#endif // WORD_H
