#include "screen.h"
#include <QDebug>
#include <algorithm>

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
        if(dm.dmPelsWidth < dm.dmPelsHeight)
            std::swap(dm.dmPelsWidth, dm.dmPelsHeight);
        break;
    case Orientation::Portrait:
        dm.dmDisplayOrientation = DMDO_90;
        if(dm.dmPelsWidth > dm.dmPelsHeight)
            std::swap(dm.dmPelsWidth, dm.dmPelsHeight);
        break;
    case Orientation::LandscapeFlip:
        dm.dmDisplayOrientation = DMDO_180;
        if(dm.dmPelsWidth < dm.dmPelsHeight)
            std::swap(dm.dmPelsWidth, dm.dmPelsHeight);
        break;
    case Orientation::PortraitFlip:
        dm.dmDisplayOrientation = DMDO_270;
        if(dm.dmPelsWidth > dm.dmPelsHeight)
            std::swap(dm.dmPelsWidth, dm.dmPelsHeight);
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
