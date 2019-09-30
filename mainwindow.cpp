#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent), ui(new Ui::MainWindow), scr(new Screen), iWebSocket(new InputWebSocket)
{
    ui->setupUi(this);
    bar = new QStatusBar(this);
    bar->setSizeGripEnabled(false);
    bar->setStyleSheet("font-size: 12px");
    ui->verticalLayout->addWidget(bar);
    setupSysTray();

    connect(scr, &Screen::statusChanged, this, &MainWindow::onStatusReceived);
    connect(iWebSocket, &InputWebSocket::messageToScreen, scr, &Screen::onMessageReceived);
    connect(iWebSocket, &InputWebSocket::sendStatusUpdate, this, &MainWindow::onStatusReceived);

    ui->lineEditWebSocketAddr->setText(readSettings("lastaddress").toString());
    ui->checkBoxAutostart->setChecked(readSettings("autostart_enabled").toBool());
}

MainWindow::~MainWindow()
{
    delete ui;
}

QVariant MainWindow::readSettings(QString key)
{
    QSettings settings("disorient", "disorient");
    return settings.value(key);
}

void MainWindow::writeToRegistry(QString key, QVariant value)
{
    QSettings settings("disorient", "disorient");
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
    if(url.isValid() && url.scheme() == "ws") {
        iWebSocket->setServerUrl(url);
        writeToRegistry("lastaddress", url);
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
        bootSettings.setValue("disorient", path);
        writeToRegistry("autostart_enabled", Qt::Checked);
    } else {
        bootSettings.remove("disorient");
        writeToRegistry("autostart_enabled", Qt::Unchecked);
    }
}
