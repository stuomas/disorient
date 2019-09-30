#include "screen.h"
#include "constants.h"

Screen::Screen()
{
    dm.dmSize = sizeof(DEVMODE);
    dm.dmDriverExtra = 0;
    EnumDisplaySettings(nullptr, ENUM_CURRENT_SETTINGS, &dm);
    //TODO: support multiple displays
    //This parameter is either NULL or a DISPLAY_DEVICE.DeviceName returned from EnumDisplayDevices.
    //A NULL value specifies the current display device on the computer on which the calling thread is running.
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
        long status = ChangeDisplaySettings(&dm, 0);
        switch(status) {
        case DISP_CHANGE_SUCCESSFUL:
            emit statusChanged("DISP_CHANGE_SUCCESSFUL");
            break;
        case DISP_CHANGE_BADDUALVIEW:
            emit statusChanged("DISP_CHANGE_BADDUALVIEW");
            break;
        case DISP_CHANGE_BADFLAGS:
            emit statusChanged("DISP_CHANGE_BADFLAGS");
            break;
        case DISP_CHANGE_BADMODE:
            emit statusChanged("DISP_CHANGE_BADMODE");
            break;
        case DISP_CHANGE_BADPARAM:
            emit statusChanged("DISP_CHANGE_BADPARAM");
            break;
        case DISP_CHANGE_FAILED:
            emit statusChanged("DISP_CHANGE_FAILED");
            break;
        case DISP_CHANGE_NOTUPDATED:
            emit statusChanged("DISP_CHANGE_NOTUPDATED");
            break;
        case DISP_CHANGE_RESTART:
            emit statusChanged("DISP_CHANGE_RESTART");
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
    if(msg == Names::Orientation0)
        flip(Orientation::Landscape);
    else if(msg == Names::Orientation90)
        flip(Orientation::Portrait);
    else if(msg == Names::Orientation180)
        flip(Orientation::LandscapeFlip);
    else if(msg == Names::Orientation270)
        flip(Orientation::PortraitFlip);
    else
        emit statusChanged("Message from server: " + msg);
}
