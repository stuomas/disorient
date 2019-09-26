#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent), ui(new Ui::MainWindow), scr(new Screen)
{
    ui->setupUi(this);
    connect(scr, &Screen::statusChanged, this, &MainWindow::onStatusReceived);
}

MainWindow::~MainWindow()
{
    delete ui;
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
    if(!iWebSocket) {
        iWebSocket = new InputWebSocket(url);
    } else {
        iWebSocket->setServerUrl(url);
    }
}

void MainWindow::onStatusReceived(QString status)
{
    ui->labelStatus->setText(status);
}
