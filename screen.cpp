#include "screen.h"
#include "constants.h"

Screen::Screen()
{
    //The primary display is used by default
    dm.dmSize = sizeof(dm);
    dm.dmDriverExtra = 0;
    EnumDisplaySettings(nullptr, ENUM_CURRENT_SETTINGS, &dm);
    enumerateDevices();
    chosenDisplay = allDisplayAdapters[0];
}

void Screen::enumerateDevices()
{
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

void Screen::enumerateSettings(int displayNum)
{
    if(EnumDisplaySettings(allDisplayAdapters[displayNum].DeviceName, ENUM_CURRENT_SETTINGS, &dm)) {
        chosenDisplay = allDisplayAdapters[displayNum];
    }
}

void Screen::flip(Orientation o)
{
    switch(o) {
    case Orientation::Landscape:
        dm.dmDisplayOrientation = DMDO_DEFAULT;
        adjustResolution(o, dm.dmPelsWidth, dm.dmPelsHeight);
        break;
    case Orientation::Portrait:
        dm.dmDisplayOrientation = DMDO_90;
        adjustResolution(o, dm.dmPelsWidth, dm.dmPelsHeight);
        break;
    case Orientation::LandscapeFlip:
        dm.dmDisplayOrientation = DMDO_180;
        adjustResolution(o, dm.dmPelsWidth, dm.dmPelsHeight);
        break;
    case Orientation::PortraitFlip:
        dm.dmDisplayOrientation = DMDO_270;
        adjustResolution(o, dm.dmPelsWidth, dm.dmPelsHeight);
        break;
    }

    if (dm.dmFields | DM_DISPLAYORIENTATION) {
        long status = ChangeDisplaySettingsEx(chosenDisplay.DeviceName, &dm, nullptr, 0, nullptr);
        switch(status) {
        case DISP_CHANGE_SUCCESSFUL:
            lastActionStatus = "DISP_CHANGE_SUCCESSFUL";
            break;
        case DISP_CHANGE_BADDUALVIEW:
            lastActionStatus = "DISP_CHANGE_BADDUALVIEW";
            break;
        case DISP_CHANGE_BADFLAGS:
            lastActionStatus = "DISP_CHANGE_BADFLAGS";
            break;
        case DISP_CHANGE_BADMODE:
            lastActionStatus = "DISP_CHANGE_BADMODE";
            break;
        case DISP_CHANGE_BADPARAM:
            lastActionStatus = "DISP_CHANGE_BADPARAM";
            break;
        case DISP_CHANGE_FAILED:
            lastActionStatus = "DISP_CHANGE_FAILED";
            break;
        case DISP_CHANGE_NOTUPDATED:
            lastActionStatus = "DISP_CHANGE_NOTUPDATED";
            break;
        case DISP_CHANGE_RESTART:
            lastActionStatus = "DISP_CHANGE_RESTART";
            break;
        }
    }
}

void Screen::adjustResolution(Orientation o, unsigned long &w, unsigned long &h)
{
    switch(o) {
    case Orientation::Landscape:
    case Orientation::LandscapeFlip:
        if(w < h)
            std::swap(w, h);
        break;
    case Orientation::Portrait:
    case Orientation::PortraitFlip:
        if(w > h)
            std::swap(w, h);
        break;
    }
}

void Screen::onMessageReceived(QString msg)
{
    //Validate text messages from the WebSocket server
    if(msg == Names::Orientation0) {
        flip(Orientation::Landscape);
        emit messageToLog(QString("→ %1 (%2)").arg(Names::Orientation0).arg(lastActionStatus));
    } else if(msg == Names::Orientation90) {
        flip(Orientation::Portrait);
        emit messageToLog(QString("→ %1 (%2)").arg(Names::Orientation90).arg(lastActionStatus));
    } else if(msg == Names::Orientation180) {
        flip(Orientation::LandscapeFlip);
        emit messageToLog(QString("→ %1 (%2)").arg(Names::Orientation180).arg(lastActionStatus));
    } else if(msg == Names::Orientation270) {
        flip(Orientation::PortraitFlip);
        emit messageToLog(QString("→ %1 (%2)").arg(Names::Orientation270).arg(lastActionStatus));
    } else
        emit messageToLog("Unrecognized message from server: " + msg);
}
