#ifndef CONSTANTS_H
#define CONSTANTS_H

#include <QString>

namespace Names {
//Names used in Windows registry
const QString SettingOrganization = "Disorient";
const QString SettingApplication = "Disorient";
const QString SettingFirstStart = "dis_opt_1";
const QString SettingLastWsAddress = "dis_opt_2";
const QString SettingLastMqttAddress = "dis_opt_3";
const QString SettingAutostartEnabled = "dis_opt_4";
const QString SettingShowPopups = "dis_opt_5";
const QString SettingSelectedMonitor = "dis_opt_6";
const QString SettingMqttUser = "dis_opt_7";
const QString SettingMqttPassword = "dis_opt_8";
const QString SettingMqttTopic = "dis_opt_9";
const QString SettingMqttQos = "dis_opt_10";
const QString SettingPayloadMap = "dis_opt_11";
const QString SettingRawExecPermission = "dis_opt_12";
const QString SettingPublishOutput = "dis_opt_13";
const QString SettingSaveCredentials = "dis_opt_14";

//Other stuff
const QString MqttPublishPath = "/output";
}

#endif // CONSTANTS_H
