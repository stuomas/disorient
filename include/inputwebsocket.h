#ifndef INPUTWEBSOCKET_H
#define INPUTWEBSOCKET_H

/* Disorient acts as a WebSocket client.
 * Give the WebSocket server address in the GUI
 * and it will be automatically connected to and
 * the server can start pushing text messages.
*/

#include <QString>
#include <QtWebSockets/QWebSocket>
#include <QTimer>
#include "constants.h"

class InputWebSocket : public QObject
{
    Q_OBJECT

public:
    InputWebSocket();
    ~InputWebSocket();
    void connectToServer(const QUrl &addr);
    bool validateUrl(const QUrl &url);
    void closeConnection();
    void reconnect();

signals:
    void messageToEndpoint(const QString &msg);
    void statusToLog(const QString &msg, const QString &sender = Names::InputWebSocketName);

public slots:
    void onPublish(const QString &msg, const QString &subtopic = "");
private slots:
    void onConnected();
    void onDisconnected();
    void onTextMessageReceived(const QString &msg);
    void onError(const QAbstractSocket::SocketError &error);

private:
    QWebSocket *m_wsock;
    QUrl m_serverUrl;
    QTimer *m_autoReconnectTimer = new QTimer(this);
    bool m_autoReconnectInProgress = false;
};

#endif // INPUTWEBSOCKET_H
