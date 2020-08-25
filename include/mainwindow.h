#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QStatusBar>
#include <QSystemTrayIcon>
#include <QMenu>
#include <QAction>
#include <QMessageBox>
#include <QCloseEvent>
#include <QSettings>
#include <QDir>
#include <QDateTime>
#include <QComboBox>
#include <QJsonObject>
#include <QJsonDocument>
#include "endpoint.h"
#include "inputwebsocket.h"
#include "inputmqtt.h"
#include "constants.h"
#include "windows.h"
#include "audioendpointcontroller.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
    QVariant readFromRegistry(const QString &key);
    void writeToRegistry(const QString &key, const QVariant &value);
    void loadSettingsFromRegistry();
    bool nativeEvent(const QByteArray& eventType, void* message, long* result);
    void log(const QString &logtext);
    void setupDisplayCombobox(const QVector<DISPLAY_DEVICE> &displays);
    void setupAudioCombobox(const QVector<QString> &audio);
    void setupHotkeys();
    void setupPayloadTable();
    void setupStyles();
    void savePayloadMap();
    void loadPayloadMap(const QJsonDocument &file = QJsonDocument());
    QString timestamp();
    QVector<QString> getAllAudioDevices();
    void removeFromRegistry(const QString &key);

signals:
    void mqttPublish(const QString &msg, const QString &subtopic = "");
    void websocketPublish(const QString &msg, const QString &subtopic = "");

private slots:
    void onStatusReceived(const QString &status, const QString &sender);
    void onAddRowClicked();
    void on_pushButtonSaveSettings_clicked();
    void on_checkBoxExecPermission_stateChanged(int arg1);
    void on_lineEditMqttTopic_textChanged(const QString &arg1);
    void on_pushButtonRefreshDisplays_clicked();
    void on_pushButtonRefreshAudio_clicked();
    void on_pushButtonExportPayloadMap_clicked();
    void on_pushButtonImportPayloadMap_clicked();

private:
    Ui::MainWindow *m_ui;
    Endpoint *m_endpoint;
    InputWebSocket *m_iWebSocket;
    InputMqtt *m_iMqtt;
    QString m_statusMsg;
    QSystemTrayIcon *m_sysTrayIcon;
    bool m_closing;
    bool m_firstStart;
    bool m_firstHide = true;
    QJsonObject m_payloadMap;
    AudioEndpointController *m_audioDevice;
    void setupSysTray();

protected:
    void closeEvent(QCloseEvent *event);
};

#endif // MAINWINDOW_H
