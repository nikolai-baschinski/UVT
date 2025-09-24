#include "word.h"

Word::Word() {}

bool Word::operator==(const Word& word_to_compare_with) const
{
    QString thisCompareString = this->foreign;
    for (const UVT::Native& native : this->natives) {
        thisCompareString += native.native;
        thisCompareString += native.example;
    }

    QString paramCompareString = word_to_compare_with.foreign;
    for (const UVT::Native& native : word_to_compare_with.natives) {
        paramCompareString += native.native;
        paramCompareString += native.example;
    }

    return thisCompareString == paramCompareString ? true : false;
}
bool Word::operator!=(const Word& word_to_compare_with) const
{
    return !(*this == word_to_compare_with);
}
