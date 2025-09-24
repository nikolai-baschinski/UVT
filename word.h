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

    bool operator==(const Word& word_to_compare_with) const;
    bool operator!=(const Word& word_to_compare_with) const;
};

#endif // WORD_H
