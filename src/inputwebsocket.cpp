#include "inputwebsocket.h"

InputWebSocket::InputWebSocket() : m_wsock(new QWebSocket)
{
    connect(m_wsock, &QWebSocket::connected, this, &InputWebSocket::onConnected);
    connect(m_wsock, &QWebSocket::disconnected, this, &InputWebSocket::onDisconnected);
    connect(m_wsock, QOverload<QAbstractSocket::SocketError>::of(&QWebSocket::error), [=](QAbstractSocket::SocketError error) {
        emit statusToLog(m_wsock->errorString() + QString(" (error code %1)").arg(error));
    });

    //Does not work with static linking yet...
    //QSslConfiguration sslConfiguration = m_wsock->sslConfiguration();
    //sslConfiguration.setPeerVerifyMode(QSslSocket::VerifyPeer);
    //m_wsock->setSslConfiguration(sslConfiguration);
}

InputWebSocket::~InputWebSocket()
{
    m_wsock->close();
}

void InputWebSocket::connectToServer(const QUrl &addr)
{
    if(m_wsock->state() == QAbstractSocket::ConnectedState && addr == m_serverUrl) return;

    if(!addr.isEmpty() && validateUrl(addr)) {
        m_serverUrl = addr;
        m_wsock->open(m_serverUrl);
    } else if(!validateUrl(addr)) {
        emit statusToLog("Invalid WebSocket address!");
    }
}

bool InputWebSocket::validateUrl(QUrl url)
{
    return url.isEmpty() || (url.isValid() && (url.scheme() == "ws" || url.scheme() == "wss"));
}

void InputWebSocket::closeConnection()
{
    m_wsock->close(QWebSocketProtocol::CloseCodeNormal, "User closed application");
}

void InputWebSocket::onConnected()
{ 
    connect(m_wsock, &QWebSocket::textMessageReceived, this, &InputWebSocket::onTextMessageReceived);
    m_wsock->sendTextMessage(QStringLiteral("Hello, server!"));
    emit statusToLog("<font color='green'>Connected to WebSocket server!</font>");
}

void InputWebSocket::onDisconnected()
{
    m_wsock->sendTextMessage(QStringLiteral("Client exited"));
    emit statusToLog("<font color='red'>Disconnected from WebSocket server.</font>");
}

void InputWebSocket::onTextMessageReceived(QString msg)
{
    emit messageToScreen(msg);
}
