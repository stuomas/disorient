#ifndef SCREEN_H
#define SCREEN_H

#include <QString>
#include <QObject>
#include <QDebug>
#include <algorithm>
#include "windows.h"

//Naming assumes landscape-native device (desktop PC)
enum class Orientation {Landscape, Portrait, LandscapeFlip, PortraitFlip};

class Screen : public QObject
{
    Q_OBJECT

public:
    Screen();
    void flip(Orientation o);
    void adjustResolution(Orientation o, unsigned long &w, unsigned long &h);
signals:
    void statusChanged(QString status);
    void messageToLog(QString msg);

public slots:
    void onMessageReceived(QString msg);
private:
    DEVMODE dm;
    QString lastActionStatus;
};

#endif // SCREEN_H
