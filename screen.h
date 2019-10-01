#ifndef SCREEN_H
#define SCREEN_H

#include <QString>
#include <QObject>
#include <QDebug>
#include <QVector>
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
    void enumerateDevices();
    void enumerateSettings(int chosenDisplay);
    QVector<DISPLAY_DEVICE> getDisplays() {return displays;}

signals:
    void statusChanged(QString status);
    void messageToLog(QString msg);

public slots:
    void onMessageReceived(QString msg);

private:
    DEVMODE dm;
    QString lastActionStatus;
    QVector<DISPLAY_DEVICE> displays;
};

#endif // SCREEN_H
