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
    InputWebSocket(const QUrl &addr);
    ~InputWebSocket();
    void setServerUrl(const QUrl &addr);
signals:
    void closed();

private slots:
    void onConnected();
    void onTextMessageReceived(QString msg);

private:
    QWebSocket *wsock;
    QUrl serverUrl;
};

#endif // INPUTWEBSOCKET_H
