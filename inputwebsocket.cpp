#include "inputwebsocket.h"
#include <QDebug>

InputWebSocket::InputWebSocket(const QUrl &addr) : wsock(new QWebSocket)
{
    connect(wsock, &QWebSocket::connected, this, &InputWebSocket::onConnected);
    connect(wsock, &QWebSocket::disconnected, this, &InputWebSocket::closed);
    connect(wsock, &QWebSocket::textMessageReceived, this, &InputWebSocket::onTextMessageReceived);
    wsock->open(addr);
}

InputWebSocket::~InputWebSocket()
{
    wsock->close();
}

void InputWebSocket::setServerUrl(const QUrl &addr)
{
    serverUrl = addr;
}

void InputWebSocket::onConnected()
{ 
    wsock->sendTextMessage(QStringLiteral("Hello, world"));
}

void InputWebSocket::onTextMessageReceived(QString msg)
{
    // - Validate message
    // - Run flip function with validated message
    //      scr->flip(msg);
    // - One erroneous msg, print GUI error (and CLI later)
    qDebug() << msg;
}


