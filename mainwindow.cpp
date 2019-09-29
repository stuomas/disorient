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
    ui->lineEditWebSocketAddr->setText(readSettings().toString());
}

MainWindow::~MainWindow()
{
    delete ui;
}

QUrl MainWindow::readSettings()
{
    QSettings settings("disorient", "disorient");
    return settings.value("lastserver").toUrl();
}

void MainWindow::writeSettings(QUrl server)
{
    QSettings settings("disorient", "disorient");
    settings.setValue("lastserver", server);
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
    iWebSocket->setServerUrl(url);
    writeSettings(url);
}

void MainWindow::onStatusReceived(QString status)
{
    bar->showMessage(status);
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
