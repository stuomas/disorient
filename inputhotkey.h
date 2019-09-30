#ifndef INPUTHOTKEY_H
#define INPUTHOTKEY_H

#include <QObject>
#include <QWidget>
#include "windows.h"

class inputhotkey
{
public:
    inputhotkey();
    int registerHotkeys();
};

#endif // INPUTHOTKEY_H
