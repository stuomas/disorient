#ifndef SCREEN_H
#define SCREEN_H

#include "windows.h"
#include <QString>
#include <QObject>

//Naming assumes landscape-native device (desktop PC)
enum class Orientation {Landscape, Portrait, LandscapeFlip, PortraitFlip};

class Screen : public QObject
{
    Q_OBJECT

public:
    Screen();
    void flip(Orientation o);
signals:
    void statusChanged(QString status);
private:
    DEVMODE dm;
};

#endif // SCREEN_H
