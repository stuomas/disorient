#include "inputwebsocket.h"

InputWebSocket::InputWebSocket() : wsock(new QWebSocket)
{
    connect(wsock, &QWebSocket::connected, this, &InputWebSocket::onConnected);
    connect(wsock, &QWebSocket::disconnected, this, &InputWebSocket::closed);
    connect(wsock, &QWebSocket::textMessageReceived, this, &InputWebSocket::onTextMessageReceived);
}

InputWebSocket::~InputWebSocket()
{
    wsock->close();
}

void InputWebSocket::connectToServer(const QUrl &addr)
{
    serverUrl = addr;
    wsock->open(addr);
}

bool InputWebSocket::validateUrl(QUrl url)
{
    return url.isValid() && url.scheme() == "ws";
}

void InputWebSocket::closeConnection()
{
    wsock->close(QWebSocketProtocol::CloseCodeNormal, "User closed application");
    qDebug() << "user closed";
}

void InputWebSocket::onConnected()
{ 
    wsock->sendTextMessage(QStringLiteral("Hello, server!"));
    emit sendStatusUpdate("Connected to WebSocket server!");
}

void InputWebSocket::onTextMessageReceived(QString msg)
{
    qDebug() << msg;
    emit messageToScreen(msg);
}
