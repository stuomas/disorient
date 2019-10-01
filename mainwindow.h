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
#include "screen.h"
#include "inputwebsocket.h"
#include "constants.h"
#include "windows.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
    QVariant readSettings(QString key);
    void writeToRegistry(QString key, QVariant value);
    bool nativeEvent(const QByteArray& eventType, void* message, long* result);
    void setupSettings();
    void log(const QString &logtext);
    void setupCombobox(QVector<DISPLAY_DEVICE> displays);
    QString timestamp();

private slots:
    void on_pbToLandscape_clicked();
    void on_pbToPortrait_clicked();
    void on_pbToLandscapeFlipped_clicked();
    void on_pbToPortraitFlipped_clicked();
    void on_lineEditWebSocketAddr_returnPressed();
    void onStatusReceived(QString status);
    void on_checkBoxAutostart_stateChanged(int state);

private:
    Ui::MainWindow *ui;
    Screen *scr;
    InputWebSocket *iWebSocket;
    QString statusMsg;
    QSystemTrayIcon *sysTrayIcon;
    bool closing;
    void setupSysTray();

protected:
    void closeEvent(QCloseEvent *event);
};

#endif // MAINWINDOW_H
