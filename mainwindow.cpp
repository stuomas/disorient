#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent), ui(new Ui::MainWindow), scr(new Screen), iWebSocket(new InputWebSocket)
{
    ui->setupUi(this);
    bar = new QStatusBar(this);
    bar->setStyleSheet("font-size: 12px");
    ui->verticalLayout->addWidget(bar);
    setupSysTray();
    setupSettings();

    connect(scr, &Screen::statusChanged, this, &MainWindow::onStatusReceived);
    connect(iWebSocket, &InputWebSocket::messageToScreen, scr, &Screen::onMessageReceived);
    connect(iWebSocket, &InputWebSocket::sendStatusUpdate, this, &MainWindow::onStatusReceived);

    //TODO: Add error handling
    RegisterHotKey(HWND(winId()), 101, MOD_CONTROL | MOD_ALT, VK_UP);
    RegisterHotKey(HWND(winId()), 102, MOD_CONTROL | MOD_ALT, VK_RIGHT);
    RegisterHotKey(HWND(winId()), 103, MOD_CONTROL | MOD_ALT, VK_DOWN);
    RegisterHotKey(HWND(winId()), 104, MOD_CONTROL | MOD_ALT, VK_LEFT);
}

MainWindow::~MainWindow()
{
    iWebSocket->closeConnection();
    delete ui;
}

QVariant MainWindow::readSettings(QString key)
{
    QSettings settings(Names::SettingOrganization, Names::SettingApplication);
    return settings.value(key);
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
    bar->showMessage(status);
    if(!this->isActiveWindow())
        sysTrayIcon->showMessage(QString("Message from server"), status);
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
    sysTrayIcon->setIcon(QIcon(":/icon3.ico"));
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

void MainWindow::setupSettings()
{
    QUrl lastUrl = readSettings(Names::SettingLastAddress).toUrl();
    if(iWebSocket->validateUrl(lastUrl)) {
        ui->lineEditWebSocketAddr->setText(lastUrl.toString());
        iWebSocket->connectToServer(lastUrl);
    }
    ui->checkBoxAutostart->setChecked(readSettings(Names::SettingAutostartEnabled).toBool());
}
