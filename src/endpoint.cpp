#include "endpoint.h"
#include "constants.h"
#include <QProcess>
#include <QJsonArray>
#include <QDir>
#include <QUrlQuery>

Endpoint::Endpoint()
{
    //The primary display is used by default
    dm.dmSize = sizeof(dm);
    dm.dmDriverExtra = 0;
    EnumDisplaySettings(nullptr, ENUM_CURRENT_SETTINGS, &dm);
    enumerateDevices();
    chosenDisplay = allDisplayAdapters[0];
}

void Endpoint::enumerateDevices()
{
    allDisplayMonitors.clear();
    allDisplayAdapters.clear();

    DISPLAY_DEVICE ddev;
    ddev.cb = sizeof(ddev);
    ddev.StateFlags = DISPLAY_DEVICE_ACTIVE;
    DWORD devNum = 0;

    while(EnumDisplayDevices(nullptr, devNum, &ddev, EDD_GET_DEVICE_INTERFACE_NAME)) {
        DISPLAY_DEVICE dmon;
        dmon.cb = sizeof(dmon);
        allDisplayAdapters.push_back(ddev);
        if(EnumDisplayDevices(ddev.DeviceName, 0, &dmon, 0)) { //Second call needed to get monitor name
            allDisplayMonitors.push_back(dmon);
        }
        ++devNum;
    }
}

void Endpoint::enumerateSettings(int displayNum)
{
    if(EnumDisplaySettings(allDisplayAdapters[displayNum].DeviceName, ENUM_CURRENT_SETTINGS, &dm)) {
        chosenDisplay = allDisplayAdapters[displayNum];
    }
}

QString Endpoint::getWinApiStatus(LONG status)
{
    switch(status) {
    case DISP_CHANGE_SUCCESSFUL:
        return "DISP_CHANGE_SUCCESSFUL";
    case DISP_CHANGE_BADDUALVIEW:
        return "DISP_CHANGE_BADDUALVIEW";
    case DISP_CHANGE_BADFLAGS:
        return "DISP_CHANGE_BADFLAGS";
    case DISP_CHANGE_BADMODE:
        return "DISP_CHANGE_BADMODE";
    case DISP_CHANGE_BADPARAM:
        return "DISP_CHANGE_BADPARAM";
    case DISP_CHANGE_FAILED:
        return "DISP_CHANGE_FAILED";
    case DISP_CHANGE_NOTUPDATED:
        return "DISP_CHANGE_NOTUPDATED";
    case DISP_CHANGE_RESTART:
        return "DISP_CHANGE_RESTART";
    default:
        return "Unknown status";
    }
}

QString Endpoint::flip(int angle)
{
    switch(angle) {
    case 0:
        dm.dmDisplayOrientation = DMDO_DEFAULT;
        adjustResolution(angle, dm.dmPelsWidth, dm.dmPelsHeight);
        break;
    case 90:
        dm.dmDisplayOrientation = DMDO_90;
        adjustResolution(angle, dm.dmPelsWidth, dm.dmPelsHeight);
        break;
    case 180:
        dm.dmDisplayOrientation = DMDO_180;
        adjustResolution(angle, dm.dmPelsWidth, dm.dmPelsHeight);
        break;
    case 270:
        dm.dmDisplayOrientation = DMDO_270;
        adjustResolution(angle, dm.dmPelsWidth, dm.dmPelsHeight);
        break;
    default:
        return "Invalid angle";
    }

    if(dm.dmFields | DM_DISPLAYORIENTATION) {
        return getWinApiStatus(ChangeDisplaySettingsEx(chosenDisplay.DeviceName, &dm, nullptr, 0, nullptr));
    } else {
        return "Unknown error";
    }
}

QString Endpoint::rearrangeDisplays(int idxPrimary, int idxSecondary)
{
    DEVMODE dmPri, dmSec;
    dmPri.dmSize = sizeof(dmPri);
    dmPri.dmDriverExtra = 0;
    dmSec.dmSize = sizeof(dmSec);
    dmSec.dmDriverExtra = 0;

    EnumDisplaySettings(allDisplayAdapters[idxPrimary].DeviceName, ENUM_CURRENT_SETTINGS, &dmPri);
    EnumDisplaySettings(allDisplayAdapters[idxSecondary].DeviceName, ENUM_CURRENT_SETTINGS, &dmSec);

    qDebug() << "Primary" << allDisplayAdapters[idxPrimary].DeviceName;
    qDebug() << "Secondary" << allDisplayAdapters[idxSecondary].DeviceName;
    //Save position of current secondary monitor
    auto oldPosX = dmPri.dmPosition.x;
    auto oldPosY = dmPri.dmPosition.y;

    //New primary, must be at position (0,0)
    dmPri.dmFields = DM_POSITION;
    dmPri.dmPosition.x = 0;
    dmPri.dmPosition.y = 0;

    //New secondary, use position of previous secondary monitor (negated) to keep same arrangement as set in Windows display properties
    dmSec.dmFields = DM_POSITION;
    dmSec.dmPosition.x = -oldPosX;
    dmSec.dmPosition.y = -oldPosY;

    LONG status = ChangeDisplaySettingsEx(allDisplayAdapters[idxPrimary].DeviceName, &dmPri, NULL, CDS_SET_PRIMARY | CDS_UPDATEREGISTRY | CDS_NORESET, NULL);
    ChangeDisplaySettingsEx(allDisplayAdapters[idxSecondary].DeviceName, &dmSec, NULL, CDS_UPDATEREGISTRY | CDS_NORESET, NULL);
    ChangeDisplaySettingsEx(0,0,0,0,0);

    return getWinApiStatus(status);
}

void Endpoint::adjustResolution(int angle, unsigned long &w, unsigned long &h)
{
    switch(angle) {
    case 0:
    case 180:
        if(w < h)
            std::swap(w, h);
        break;
    case 90:
    case 270:
        if(w > h)
            std::swap(w, h);
        break;
    }
}

//TODO: support multiple (comma-separated) payloads in one message
void Endpoint::onMessageReceived(const QString &msg)
{
    QString origin = sender()->objectName();
    QString lastActionStatus, functionName, functionArg, payloadName;
    QStringList wildcardList;
    QStringList splitMsg = msg.split("?");
    QString baseMsg = splitMsg.at(0);
    QMap<QString, QString> wildcardMap;
    QRegExp queryPairDelimiter("(\\,|\\&)"); // accepts '&' and ',' between pairs

    if(splitMsg.size() > 1) {
        wildcardList = splitMsg.at(1).split(queryPairDelimiter);
        for(auto str : wildcardList) {
            if(str.split("=").size() > 1) {
                wildcardMap.insert("$" + str.split("=").at(0).trimmed(), str.split("=").at(1).trimmed());
            }
        }
    }

    qDebug() << wildcardMap;

    QJsonObject payloads = m_payloadMap;
    QJsonArray payloadArr = payloads.value("payload").toArray();
    QJsonArray functionArr = payloads.value("function").toArray();
    QJsonArray argumentArr = payloads.value("argument").toArray();
    bool unrecognizedMsg = false;

    for(int i = 0; i < payloadArr.size(); ++i) {
        if(payloadArr[i].toString() == baseMsg) {
            functionName = functionArr[i].toString();
            functionArg = argumentArr[i].toString();
            payloadName = payloadArr[i].toString();
        }
    }

    QStringList args = functionArg.split(",");
    for(auto& str : args) {
        str = str.trimmed();
        str.replace("$$", payloadName); //Replace $$ in arguments with the payloadName, might be useful
        if(m_wildcardsAllowed) {
            for(auto wc : wildcardMap.toStdMap()) {
                str.replace(wc.first, wc.second);
            }
        }
    }

    /************************************************************************
    ** Unrecognized message when functionName and payloadName are still empty
    *************************************************************************/
    if(functionName.isEmpty() && payloadName.isEmpty()) {
        unrecognizedMsg = true;
        emit statusToLog("Unrecognized message: " + msg, origin);
    }
    /************************************************************************
    ** Rotate screen (index, angle)
    *************************************************************************/
    else if(functionName == Names::Functions.at(1)) {
        if(args.size() < 2) {
            lastActionStatus = "Invalid arguments";
        } else {
            bool ok;
            int angle = args.at(1).toInt(&ok);
            lastActionStatus = "Invalid angle";
            if(ok) {
                enumerateSettings(args.at(0).toInt());
                lastActionStatus = flip(angle);
            }
        }
    }
    /************************************************************************
    ** Set audio device (name)
    *************************************************************************/
    else if(functionName == Names::Functions.at(2)) {
        emit changeAudioDevice(functionArg);
    }
    /************************************************************************
    ** Arrange displays (index1, index2)
    *************************************************************************/
    else if(functionName == Names::Functions.at(3)) {
        if(args.size() < 2) {
            lastActionStatus = "Invalid arguments";
        } else {
            lastActionStatus = rearrangeDisplays(args.at(0).toInt(), args.at(1).toInt());
        }
    }
    /************************************************************************
    ** Run executable (path)
    *************************************************************************/
    else if(functionName == Names::Functions.at(4)) {
        QString path = args.at(0);
        if(path.endsWith(".ps1") || path.endsWith(".bat") || path.endsWith(".cmd")) {
            lastActionStatus = runInPowershell(args);
        } else {
            args.takeFirst();
            QProcess::startDetached(path, args);
        }
    }

    if(m_rawExecPermission && unrecognizedMsg && !msg.trimmed().isEmpty()) {
        emit statusToLog("Attempting to execute in PowerShell");
        QString output = runInPowershell({msg});
        emit statusToLog("Output: " + output);
        if(m_rawExecPublish) {
            sendResponse(origin, output, Names::MqttPowershellSubtopic);
        }
    } else if(!unrecognizedMsg) {
        emit statusToLog(QString("%1 (%2)").arg(payloadName).arg(lastActionStatus), origin);
        sendResponse(origin, lastActionStatus, QString("%1/%2").arg(payloadName).arg(Names::MqttResponseSubtopic));
    } else {
        sendResponse(origin, "Unrecognized message", QString("%1/%2").arg(msg).arg(Names::MqttResponseSubtopic));
    }
}

QString Endpoint::runInPowershell(const QStringList &args)
{
    QProcess powershell;
    QString cmd("powershell");
    QStringList parameters{args};
    powershell.setProcessChannelMode(QProcess::MergedChannels);
    powershell.setReadChannel(QProcess::StandardOutput);
    powershell.start(cmd, parameters, QIODevice::ReadWrite);
    powershell.waitForFinished(3000);
    QString stdOut = powershell.readAllStandardOutput().trimmed();
    powershell.close();
    return stdOut;
}

void Endpoint::sendResponse(const QString &sender, const QString &response, const QString &topic)
{
    if(sender == "InputMqtt") {
        emit mqttPublish(response, topic);
    } else if(sender == "InputWebSocket") {
        emit websocketPublish(response, topic);
    }
}

