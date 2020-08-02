#include "inputwebsocket.h"

InputWebSocket::InputWebSocket() : m_wsock(new QWebSocket)
{
    connect(m_wsock, &QWebSocket::connected, this, &InputWebSocket::onConnected);
    connect(m_wsock, &QWebSocket::disconnected, this, &InputWebSocket::onDisconnected);
    connect(m_wsock, QOverload<QAbstractSocket::SocketError>::of(&QWebSocket::error), this, &InputWebSocket::onError);
    connect(m_autoReconnectTimer, &QTimer::timeout, this, &InputWebSocket::reconnect);

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

    m_serverUrl = addr;

    if(!addr.isEmpty() && validateUrl(addr)) {
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

void InputWebSocket::reconnect()
{
    m_autoReconnectInProgress = true;
    m_wsock->open(m_serverUrl);
}

void InputWebSocket::onConnected()
{
    connect(m_wsock, &QWebSocket::textMessageReceived, this, &InputWebSocket::onTextMessageReceived);
    m_wsock->sendTextMessage(QStringLiteral("Hello, server!"));
    emit statusToLog("<font color='green'>Connected to WebSocket server!</font>");
    m_autoReconnectInProgress = false;
}

void InputWebSocket::onDisconnected()
{
    if(!m_autoReconnectInProgress) {
        emit statusToLog("<font color='red'>Disconnected from WebSocket server.</font>");
    } else {
        qDebug() << "WebSocket autoreconnecting...";
    }
    if(!m_serverUrl.isEmpty()) {
        m_autoReconnectTimer->setInterval(10000);
        m_autoReconnectTimer->setSingleShot(true);
        m_autoReconnectTimer->start();
    }
}

void InputWebSocket::onTextMessageReceived(QString msg)
{
    emit messageToScreen(msg);
}

void InputWebSocket::onError(QAbstractSocket::SocketError error)
{
    if(!m_autoReconnectInProgress) {
        emit statusToLog(m_wsock->errorString() + QString(" (error code %1)").arg(error));
    }
}
