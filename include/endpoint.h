#ifndef SCREEN_H
#define SCREEN_H

#include <QString>
#include <QObject>
#include <QVector>
#include <QJsonObject>
#include <algorithm>
#include "windows.h"

class Endpoint : public QObject
{
    Q_OBJECT

public:
    Endpoint();
    QString flip(int angle);
    QString rearrangeDisplays(int idxPrimary, int idxSecondary);
    void adjustResolution(int angle, unsigned long &w, unsigned long &h);
    void enumerateDevices();
    void enumerateSettings(int displayNum);
    QVector<DISPLAY_DEVICE> getDisplays() { return allDisplayMonitors; }
    void setChosenDisplay(int displayNum) { chosenDisplay = allDisplayAdapters[displayNum]; }
    void setRawExecPermission(bool setting) { m_rawExecPermission = setting; }
    void setRawExecPublish(bool setting) { m_rawExecPublish = setting; }
    void setPayloadMap(QJsonObject payload) { m_payloadMap = payload; }
    QString getWinApiStatus(LONG status);
    void sendResponse(const QString &sender, const QString &topic, const QString &response);

signals:
    void statusToLog(const QString &status, const QString &sender = "");
    void mqttPublish(const QString &msg, const QString &subtopic = "");
    void websocketPublish(const QString &msg, const QString &subtopic = "");
    void changeAudioDevice(const QString &dev);

public slots:
    void onMessageReceived(const QString &msg);

private:
    DEVMODE dm;
    bool m_rawExecPermission = false;
    bool m_rawExecPublish = false;
    QJsonObject m_payloadMap;

    //Some weird WinAPI quirks, maybe there is a better way
    QVector<DISPLAY_DEVICE> allDisplayMonitors;
    QVector<DISPLAY_DEVICE> allDisplayAdapters;
    DISPLAY_DEVICE chosenDisplay;
};

#endif // SCREEN_H
