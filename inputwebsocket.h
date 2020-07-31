#ifndef INPUTWEBSOCKET_H
#define INPUTWEBSOCKET_H

/* Disorient acts as a WebSocket client.
 * Give the WebSocket server address in the GUI
 * and it will be automatically connected to and
 * the server can start pushing text messages.
*/

#include <QString>
#include <QtWebSockets/QWebSocket>

class InputWebSocket : public QObject
{
    Q_OBJECT

public:
    InputWebSocket();
    ~InputWebSocket();
    void connectToServer(const QUrl &addr);
    bool validateUrl(QUrl url);
    void closeConnection();
signals:
    void messageToScreen(QString msg);
    void statusToLog(QString msg);

private slots:
    void onConnected();
    void onDisconnected();
    void onTextMessageReceived(QString msg);

private:
    QWebSocket *m_wsock;
    QUrl m_serverUrl;
};

#endif // INPUTWEBSOCKET_H
