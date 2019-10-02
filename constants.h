#ifndef CONSTANTS_H
#define CONSTANTS_H

#include <QString>

namespace Names {
//Names used in Windows registry
const QString SettingOrganization = "Disorient";
const QString SettingApplication = "Disorient";
const QString SettingLastAddress = "last_address";
const QString SettingAutostartEnabled = "autostart_enabled";
const QString SettingSelectedMonitor = "selected_monitor";
const QString SettingSelectedComPort = "selected_com";
const QString Orientation0 = "landscape";
const QString Orientation90 = "portrait";
const QString Orientation180 = "flandscape";
const QString Orientation270 = "fportrait";
}

namespace Tooltip {
const QString WsHelpIcon =
        QString("The address of the WebSocket server you "
                "are running somewhere and from where you "
                "want to send commands. Send commands "
                "as text frames. Expected text frames are: \n\n"
                "%1\n%2\n%3\n%4\n")
                .arg(Names::Orientation0)
                .arg(Names::Orientation90)
                .arg(Names::Orientation180)
                .arg(Names::Orientation270);

const QString ComHelpIcon = "COM port from which serial communication "
                            "is expected from. Not yet implemented.";
}
#endif // CONSTANTS_H
