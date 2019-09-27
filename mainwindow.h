#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QStatusBar>
#include "screen.h"
#include "inputwebsocket.h"
#include <QSystemTrayIcon>
#include <QMenu>
#include <QAction>
#include <QMessageBox>
#include <QCloseEvent>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void on_pbToLandscape_clicked();
    void on_pbToPortrait_clicked();
    void on_pbToLandscapeFlipped_clicked();
    void on_pbToPortraitFlipped_clicked();
    void on_lineEditWebSocketAddr_returnPressed();
    void onStatusReceived(QString status);

private:
    Ui::MainWindow *ui;
    Screen *scr;
    InputWebSocket *iWebSocket;
    QString statusMsg;
    QStatusBar *bar;
    QSystemTrayIcon *sysTrayIcon;
    bool closing;
    void setupSysTray();

protected:
    void closeEvent(QCloseEvent *event);
};

#endif // MAINWINDOW_H
