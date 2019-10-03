#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "constants.h"

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent), ui(new Ui::MainWindow), scr(new Screen), iWebSocket(new InputWebSocket)
{  
    connect(scr, &Screen::statusChanged, this, &MainWindow::onStatusReceived);
    connect(scr, &Screen::messageToLog, this, &MainWindow::log);
    connect(iWebSocket, &InputWebSocket::messageToScreen, scr, &Screen::onMessageReceived);
    connect(iWebSocket, &InputWebSocket::statusToScreen, this, &MainWindow::onStatusReceived);

    ui->setupUi(this);
    setupSysTray();
    setupCombobox(scr->getDisplays());
    setupTooltips();
    setupHotkeys();
    loadSettingsFromRegistry();

    log(QString("Welcome to %1!").arg(Names::SettingApplication));

    if(firstStart) {
        show(); //start minimized to system tray after first start
        writeToRegistry(Names::SettingFirstStart, false);
    }
}

MainWindow::~MainWindow()
{
    iWebSocket->closeConnection();
    delete ui;
}

QString MainWindow::timestamp()
{
    return QDateTime::currentDateTime().toString("HH:mm:ss");
}

void MainWindow::log(const QString &logtext)
{
    ui->textLog->append(QString("<b>[%1]</b> %2").arg(timestamp()).arg(logtext));
    ui->textLog->repaint();
}

QVariant MainWindow::readSettings(QString key)
{
    QSettings settings(Names::SettingOrganization, Names::SettingApplication);
    return settings.value(key, -1); //If setting doesn't exist, return -1
}

void MainWindow::writeToRegistry(QString key, QVariant value)
{
    QSettings settings(Names::SettingOrganization, Names::SettingApplication);
    settings.setValue(key, value);
}

void MainWindow::on_pbToLandscape_clicked()
{
    scr->flip(Orientation::Landscape);
}

void MainWindow::on_pbToPortrait_clicked()
{
    scr->flip(Orientation::Portrait);
}

void MainWindow::on_pbToLandscapeFlipped_clicked()
{
    scr->flip(Orientation::LandscapeFlip);
}

void MainWindow::on_pbToPortraitFlipped_clicked()
{
    scr->flip(Orientation::PortraitFlip);
}

void MainWindow::on_lineEditWebSocketAddr_returnPressed()
{
    QUrl url = QUrl(ui->lineEditWebSocketAddr->text());
    if(iWebSocket->validateUrl(url)) {
        iWebSocket->connectToServer(url);
        writeToRegistry(Names::SettingLastAddress, url);
    } else {
        onStatusReceived("Invalid websocket address");
    }
}

void MainWindow::onStatusReceived(QString status)
{
    log(status);
    sysTrayIcon->showMessage("Status update", status);
    sysTrayIcon->setToolTip(QString("%1\n%2").arg(Names::SettingApplication).arg(status));
}

void MainWindow::setupCombobox(QVector<DISPLAY_DEVICE> displays)
{
    int n = 1;
    for(auto i : displays) {
        ui->comboBoxDisplayList->addItem(QString::number(n) + QString(". ") + QString::fromWCharArray(i.DeviceString));
        ++n;
    }
}

void MainWindow::setupTooltips()
{
    ui->lineEditWebSocketAddr->setToolTip(Tooltip::WsHelpIcon);
    ui->comboBoxCOM->setToolTip(Tooltip::ComHelpIcon);
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
    closing = false;
    auto exitAction = new QAction(tr("&Exit"), this);
    connect(exitAction, &QAction::triggered, [this]() {
        closing = true;
        close();
    });

    auto trayIconMenu = new QMenu(this);
    trayIconMenu->addAction(exitAction);

    sysTrayIcon = new QSystemTrayIcon(this);
    sysTrayIcon->setContextMenu(trayIconMenu);
    sysTrayIcon->setIcon(QIcon(":/icon.ico"));
    sysTrayIcon->show();

    connect(sysTrayIcon, &QSystemTrayIcon::activated, [this](auto reason) {
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
    if(closing) {
        event->accept();
    }
    else {
        this->hide();
        event->ignore();
        if(firstHide) {
            firstHide = false;
            sysTrayIcon->showMessage("Still here!", "Select Exit from the right-click menu to exit.");
        }
    }
}

void MainWindow::on_checkBoxAutostart_stateChanged(int state)
{
    QSettings bootSettings("HKEY_LOCAL_MACHINE\\SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Run", QSettings::NativeFormat);
    QString path = QDir::toNativeSeparators(QCoreApplication::applicationFilePath());

    if(state == Qt::Checked) {
        bootSettings.setValue(Names::SettingApplication, path);
        writeToRegistry(Names::SettingAutostartEnabled, Qt::Checked);
    } else {
        bootSettings.remove(Names::SettingApplication);
        writeToRegistry(Names::SettingAutostartEnabled, Qt::Unchecked);
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
            scr->flip(Orientation::Landscape);
            break;
        case 102:
            scr->flip(Orientation::Portrait);
            break;
        case 103:
            scr->flip(Orientation::LandscapeFlip);
            break;
        case 104:
            scr->flip(Orientation::PortraitFlip);
            break;
        }
        return true;
    }
    return false;
}

void MainWindow::loadSettingsFromRegistry()
{
    //Setting for last known WebSocket address
    QUrl lastUrl = readSettings(Names::SettingLastAddress).toUrl();
    if(iWebSocket->validateUrl(lastUrl)) {
        ui->lineEditWebSocketAddr->setText(lastUrl.toString());
        iWebSocket->connectToServer(lastUrl);
    }
    //Setting for automatic startup
    ui->checkBoxAutostart->setChecked(readSettings(Names::SettingAutostartEnabled).toBool());

    //Setting for last selected display
    int index = readSettings(Names::SettingSelectedMonitor).toInt();
    ui->comboBoxDisplayList->setCurrentIndex(index);
    scr->setChosenDisplay(index);

    //Setting for first start flag
    firstStart = readSettings(Names::SettingFirstStart).toBool();
}

void MainWindow::on_comboBoxDisplayList_activated(int index)
{
    scr->enumerateSettings(index);
    writeToRegistry(Names::SettingSelectedMonitor, index);
}
