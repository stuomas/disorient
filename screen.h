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
    void enumerateSettings(int displayNum);
    QVector<DISPLAY_DEVICE> getDisplays() {return allDisplayMonitors;}
    void setChosenDisplay(int displayNum) {chosenDisplay = allDisplayAdapters[displayNum];}

signals:
    void statusChanged(QString status);
    void messageToLog(QString msg);

public slots:
    void onMessageReceived(QString msg);

private:
    DEVMODE dm;
    QString lastActionStatus;

    //Some weird WinAPI quirks, maybe there is a better way
    QVector<DISPLAY_DEVICE> allDisplayMonitors;
    QVector<DISPLAY_DEVICE> allDisplayAdapters;
    DISPLAY_DEVICE chosenDisplay;
};

#endif // SCREEN_H
