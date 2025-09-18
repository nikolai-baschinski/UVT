#ifndef TEST_SETTINGS_H
#define TEST_SETTINGS_H

enum Language {
    Foreign,
    Native,
    Mixed
};

class Test_Settings
{
public:
    Test_Settings();
public:
    unsigned int delay;
    unsigned int numberCycles;
    Language language;
};

#endif // TEST_SETTINGS_H
