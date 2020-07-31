#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "constants.h"
#include <QJsonArray>

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent), m_ui(new Ui::MainWindow), m_endpoint(new Endpoint), m_iWebSocket(new InputWebSocket), m_iMqtt(new InputMqtt), m_audioDevice(new AudioEndpointController)
{  
    connect(m_endpoint, &Endpoint::statusToLog, this, &MainWindow::onStatusReceived);
    connect(m_iWebSocket, &InputWebSocket::statusToLog, this, &MainWindow::onStatusReceived);
    connect(m_iMqtt, &InputMqtt::statusToLog, this, &MainWindow::onStatusReceived);
    connect(m_endpoint, &Endpoint::mqttPublish, m_iMqtt, &InputMqtt::onPublish);
    connect(m_endpoint, &Endpoint::changeAudioDevice, m_audioDevice, &AudioEndpointController::onChangeRequest);
    connect(m_iWebSocket, &InputWebSocket::messageToScreen, m_endpoint, &Endpoint::onMessageReceived);
    connect(m_iMqtt, &InputMqtt::messageToScreen, m_endpoint, &Endpoint::onMessageReceived);

    m_ui->setupUi(this);
    log(QString("%1 ready!").arg(Names::SettingApplication));
    setupSysTray();
    setupDisplayCombobox(m_endpoint->getDisplays());
    setupAudioCombobox(m_audioDevice->getAllAudioDevices());
    setupHotkeys();
    setupPayloadCombobox();
    loadSettingsFromRegistry();
    QFile style(":/styles.qss");
    style.open(QFile::ReadOnly);
    setStyleSheet(QString::fromLatin1(style.readAll()));
    style.close();
    m_ui->verticalLayout_3->setAlignment(Qt::AlignTop);
    m_ui->checkBoxPublishOutput->setDisabled(!m_ui->checkBoxExecPermission->isChecked());
    m_ui->checkBoxPublishOutput->setText(QString("Publish output in topic %1%2").arg(m_ui->lineEditMqttTopic->text()).arg(Names::MqttPublishPath));
    m_ui->tabWidget->setCurrentIndex(0);
    if(m_firstStart) {
        show(); //start minimized to system tray after first start
        writeToRegistry(Names::SettingFirstStart, false);
    }
}

MainWindow::~MainWindow()
{
    m_iWebSocket->closeConnection();
    delete m_ui;
}

QString MainWindow::timestamp()
{
    return QDateTime::currentDateTime().toString("HH:mm:ss");
}

QVector<QString> MainWindow::getAllAudioDevices()
{
     return m_audioDevice->getAllAudioDevices();
}

void MainWindow::log(const QString &logtext)
{
    if(logtext.isEmpty()) return;

    m_ui->textLog->append(QString("<b>[%1]</b> %2").arg(timestamp()).arg(logtext));
    m_ui->textLog->repaint();
}

QVariant MainWindow::readFromRegistry(const QString &key)
{
    QSettings settings(Names::SettingOrganization, Names::SettingApplication);
    return settings.value(key, ""); //If setting doesn't exist, return empty string
}

void MainWindow::writeToRegistry(const QString &key, const QVariant &value)
{
    QSettings settings(Names::SettingOrganization, Names::SettingApplication);
    settings.setValue(key, value);
}

void MainWindow::removeFromRegistry(const QString &key)
{
    QSettings settings(Names::SettingOrganization, Names::SettingApplication);
    settings.remove(key);
}

void MainWindow::onStatusReceived(const QString &status)
{
    log(status);
    if(m_ui->checkBoxAllowPopups->isChecked()) {
        m_sysTrayIcon->showMessage("Status update", status);
    }
    //m_sysTrayIcon->setToolTip(QString("%1\n%2").arg(Names::SettingApplication).arg(status));
}

void MainWindow::setupDisplayCombobox(const QVector<DISPLAY_DEVICE> &displays)
{
    int n = 0;
    for(auto i : displays) {
        m_ui->comboBoxDisplayList->addItem(QString::number(n) + QString(". ") + QString::fromWCharArray(i.DeviceString));
        ++n;
    }
}

void MainWindow::setupAudioCombobox(const QVector<QString> &audio)
{
    for(auto i : audio) {
        m_ui->comboBoxAudioList->addItem(i);
    }
}

void MainWindow::setupPayloadCombobox()
{
    QStringList functions = {"", "Rotate screen (index, angle)", "Set audio device (name)", "Arrange displays (index1, index2)", "Run executable (path)"};
    // Don't iterate first and last elements since they are not interesting
    for(int i = 1; i < m_ui->verticalLayoutPayloadMap->count() - 1; ++i) {
        QLayout *layout = m_ui->verticalLayoutPayloadMap->itemAt(i)->layout();
        QComboBox* functionCombo = dynamic_cast<QComboBox*>(layout->itemAt(1)->widget());
        functionCombo->addItems(functions);
    }
}

void MainWindow::setupHotkeys()
{
    if(!RegisterHotKey(HWND(winId()), 101, MOD_CONTROL | MOD_ALT, VK_UP) ||
       !RegisterHotKey(HWND(winId()), 102, MOD_CONTROL | MOD_ALT, VK_LEFT) ||
       !RegisterHotKey(HWND(winId()), 103, MOD_CONTROL | MOD_ALT, VK_DOWN) ||
       !RegisterHotKey(HWND(winId()), 104, MOD_CONTROL | MOD_ALT, VK_RIGHT)) {

        log("Some global hotkeys could not be registered.");
    }
}

void MainWindow::setupSysTray()
{
    //Adapted from amin-ahmadi.com
    m_closing = false;
    auto exitAction = new QAction(tr("&Exit"), this);
    connect(exitAction, &QAction::triggered, [this]() {
        m_closing = true;
        close();
    });

    auto trayIconMenu = new QMenu(this);
    trayIconMenu->addAction(exitAction);

    m_sysTrayIcon = new QSystemTrayIcon(this);
    m_sysTrayIcon->setContextMenu(trayIconMenu);
    m_sysTrayIcon->setIcon(QIcon(":/icons/icon.ico"));
    m_sysTrayIcon->show();

    connect(m_sysTrayIcon, &QSystemTrayIcon::activated, [this](auto reason) {
        if(reason == QSystemTrayIcon::Trigger) {
            if(isVisible()) {
                hide();
            } else {
                show();
                activateWindow();
            }
        }
    });
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    if(m_closing) {
        event->accept();
    }
    else {
        this->hide();
        event->ignore();
        if(m_firstHide) {
            m_firstHide = false;
            m_sysTrayIcon->showMessage("Still here!", "Select Exit from the right-click menu to exit.");
        }
    }
}

//Override QWidget::nativeEvent to catch registered global hotkey events
bool MainWindow::nativeEvent(const QByteArray& eventType, void* message, long* result)
{
    Q_UNUSED(eventType)
    Q_UNUSED(result)

    MSG* msg = static_cast<MSG*>(message);
    if (msg->message == WM_HOTKEY) {
        switch(msg->wParam) {
        case 101:
            m_endpoint->flip(0);
            break;
        case 102:
            m_endpoint->flip(90);
            break;
        case 103:
            m_endpoint->flip(180);
            break;
        case 104:
            m_endpoint->flip(270);
            break;
        }
        return true;
    }
    return false;
}

void MainWindow::loadSettingsFromRegistry()
{
    //Setting for last known WebSocket address
    QUrl lastWsUrl = readFromRegistry(Names::SettingLastWsAddress).toUrl();
    m_ui->lineEditWebSocketAddr->setText(lastWsUrl.toString());
    m_iWebSocket->connectToServer(lastWsUrl);

    //Setting for automatic startup
    m_ui->checkBoxAutostart->setChecked(readFromRegistry(Names::SettingAutostartEnabled).toBool());

    //Setting for popups
    m_ui->checkBoxAllowPopups->setChecked(readFromRegistry(Names::SettingShowPopups).toBool());

    //Setting for last selected display
    int index = readFromRegistry(Names::SettingSelectedMonitor).toInt();
    if(index >= 0) {
        m_ui->comboBoxDisplayList->setCurrentIndex(index);
        m_endpoint->setChosenDisplay(index);
    }

    //Setting for MQTT credentials saving
    m_ui->checkBoxSaveCredentials->setChecked(readFromRegistry(Names::SettingSaveCredentials).toBool());

    //Setting for MQTT username
    QString user = readFromRegistry(Names::SettingMqttUser).toString();
    m_ui->lineEditMqttUsername->setText(user);
    m_iMqtt->setUsername(user);

    //Setting for MQTT password
    QString pw = QString::fromLatin1(QByteArray::fromHex(readFromRegistry(Names::SettingMqttPassword).toByteArray()));
    m_ui->lineEditMqttPassword->setText(pw);
    m_iMqtt->setPassword(pw);

    //Setting for MQTT topic
    QString topic = readFromRegistry(Names::SettingMqttTopic).toString();
    m_iMqtt->setTopic(topic);
    m_ui->lineEditMqttTopic->setText(topic);

    //Setting for MQTT QoS
    int qos = readFromRegistry(Names::SettingMqttQos).toInt();
    m_ui->spinBoxMqttQos->setValue(qos);
    m_iMqtt->setQos(qos);

    //Setting for last known MQTT address
    QUrl lastMqttUrl = readFromRegistry(Names::SettingLastMqttAddress).toUrl();
    m_ui->lineEditMqttBroker->setText(lastMqttUrl.toString());
    m_iMqtt->setBroker(lastMqttUrl);

    //Raw message execution permission
    bool execPerm = readFromRegistry(Names::SettingRawExecPermission).toBool();
    m_ui->checkBoxExecPermission->setChecked(execPerm);
    m_endpoint->setRawExecPermission(execPerm);

    //Raw message output publish permission
    bool publishPerm = readFromRegistry(Names::SettingPublishOutput).toBool();
    m_ui->checkBoxPublishOutput->setChecked(publishPerm);
    m_endpoint->setRawExecPublish(publishPerm);

    //Payload map
    loadPayloadMap();

    //Setting for first start flag
    m_firstStart = readFromRegistry(Names::SettingFirstStart).toBool();
}

void MainWindow::savePayloadMap()
{
    QJsonObject payloadObject;
    QJsonArray payloadArr;
    QJsonArray functionArr;
    QJsonArray argumentArr;
    // Don't iterate first and last elements since they are not interesting
    for(int i = 1; i < m_ui->verticalLayoutPayloadMap->count() - 1; ++i) {
        QLayout *layout = m_ui->verticalLayoutPayloadMap->itemAt(i)->layout();
        QLineEdit* payload = dynamic_cast<QLineEdit*>(layout->itemAt(0)->widget());
        QComboBox* function = dynamic_cast<QComboBox*>(layout->itemAt(1)->widget());
        QLineEdit* argument = dynamic_cast<QLineEdit*>(layout->itemAt(2)->widget());
        payloadArr.push_back(payload->text());
        functionArr.push_back(function->currentIndex());
        argumentArr.push_back(argument->text());
    }
    payloadObject.insert("payload", payloadArr);
    payloadObject.insert("function", functionArr);
    payloadObject.insert("argument", argumentArr);

    m_payloadMap = payloadObject;
    m_endpoint->setPayloadMap(m_payloadMap);
}

void MainWindow::loadPayloadMap()
{
    QJsonObject payloadObject = readFromRegistry(Names::SettingPayloadMap).toJsonObject();

    auto savedArraySize = payloadObject.value("payload").toArray().count();
    auto currentMapSize = m_ui->verticalLayoutPayloadMap->count();

    if(savedArraySize < currentMapSize - 2) return;

    // Don't iterate first and last elements since they are not interesting
    for(int i = 1; i < currentMapSize - 1; ++i) {
        QLayout *layout = m_ui->verticalLayoutPayloadMap->itemAt(i)->layout();
        QLineEdit* payload = dynamic_cast<QLineEdit*>(layout->itemAt(0)->widget());
        QComboBox* function = dynamic_cast<QComboBox*>(layout->itemAt(1)->widget());
        QLineEdit* argument = dynamic_cast<QLineEdit*>(layout->itemAt(2)->widget());

        payload->setText(payloadObject.value("payload").toArray()[i-1].toString());
        function->setCurrentIndex(payloadObject.value("function").toArray()[i-1].toInt());
        argument->setText(payloadObject.value("argument").toArray()[i-1].toString());
    }
    m_payloadMap = payloadObject;
    m_endpoint->setPayloadMap(m_payloadMap);
}

void MainWindow::on_pushButtonSaveSettings_clicked()
{    
    //Autostart enabled
    QSettings bootSettings("HKEY_CURRENT_USER\\Software\\Microsoft\\Windows\\CurrentVersion\\Run", QSettings::NativeFormat);
    QString path = QDir::toNativeSeparators(QCoreApplication::applicationFilePath());

    if(m_ui->checkBoxAutostart->isChecked()) {
        writeToRegistry(Names::SettingAutostartEnabled, true);
        bootSettings.setValue(Names::SettingApplication, path);
    } else {
        writeToRegistry(Names::SettingAutostartEnabled, false);
        bootSettings.remove(Names::SettingApplication);
    }

    //Allow popups
    if(m_ui->checkBoxAllowPopups->isChecked()) {
        writeToRegistry(Names::SettingShowPopups, Qt::Checked);
    } else {
        writeToRegistry(Names::SettingShowPopups, Qt::Unchecked);
    }

    //Select display
    m_endpoint->enumerateSettings(m_ui->comboBoxDisplayList->currentIndex());
    writeToRegistry(Names::SettingSelectedMonitor, m_ui->comboBoxDisplayList->currentIndex());

    //Websocket server
    QUrl wsUrl = QUrl(m_ui->lineEditWebSocketAddr->text());
    m_iWebSocket->connectToServer(wsUrl);
    writeToRegistry(Names::SettingLastWsAddress, wsUrl);

    //Save credentials
    if(m_ui->checkBoxSaveCredentials->isChecked()) {
        writeToRegistry(Names::SettingSaveCredentials, Qt::Checked);
    } else {
        writeToRegistry(Names::SettingSaveCredentials, Qt::Unchecked);
    }

    //MQTT broker server
    QUrl mqttUrl = QUrl(m_ui->lineEditMqttBroker->text());
    m_iMqtt->setBroker(mqttUrl);
    writeToRegistry(Names::SettingLastMqttAddress, mqttUrl);

    //MQTT username
    QString user = m_ui->lineEditMqttUsername->text();
    m_iMqtt->setUsername(user);

    //MQTT password
    QString pw = m_ui->lineEditMqttPassword->text();
    m_iMqtt->setPassword(pw);

    if(m_ui->checkBoxSaveCredentials->isChecked()) {
        writeToRegistry(Names::SettingMqttUser, user);
        writeToRegistry(Names::SettingMqttPassword, pw.toLatin1().toHex()); //Save as hex just to prevent casual peeking
    } else {
        removeFromRegistry(Names::SettingMqttUser);
        removeFromRegistry(Names::SettingMqttPassword);
    }

    //MQTT topic
    QString topic = m_ui->lineEditMqttTopic->text();
    m_iMqtt->setTopic(topic);
    writeToRegistry(Names::SettingMqttTopic, topic);

    //MQTT QoS
    m_iMqtt->setQos(m_ui->spinBoxMqttQos->value());
    writeToRegistry(Names::SettingMqttQos, m_ui->spinBoxMqttQos->value());

    //Payload mapping
    savePayloadMap();
    writeToRegistry(Names::SettingPayloadMap, m_payloadMap);

    //Raw message execution permission
    m_endpoint->setRawExecPermission(m_ui->checkBoxExecPermission->isChecked());
    writeToRegistry(Names::SettingRawExecPermission, m_ui->checkBoxExecPermission->isChecked());

    //Raw message output publish permission
    m_endpoint->setRawExecPublish(m_ui->checkBoxPublishOutput->isChecked());
    writeToRegistry(Names::SettingPublishOutput, m_ui->checkBoxPublishOutput->isChecked());
}

void MainWindow::on_checkBoxExecPermission_stateChanged(int arg1)
{
    m_ui->checkBoxPublishOutput->setDisabled(!arg1);
}

void MainWindow::on_lineEditMqttTopic_textChanged(const QString &arg1)
{
    m_ui->checkBoxPublishOutput->setText(QString("Publish output in topic %1%2").arg(arg1).arg(Names::MqttPublishPath));
}

void MainWindow::on_pushButtonRefreshDisplays_clicked()
{
    m_ui->comboBoxDisplayList->clear();
    m_endpoint->enumerateDevices();
    setupDisplayCombobox(m_endpoint->getDisplays());
}

void MainWindow::on_pushButtonRefreshAudio_clicked()
{
    m_audioDevice->refreshList();
    m_ui->comboBoxAudioList->clear();
    setupAudioCombobox(m_audioDevice->getAllAudioDevices());
}
