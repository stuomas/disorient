#include "inputmqtt.h"
#include <QtMqtt/QMqttClient>

InputMqtt::InputMqtt()
{
    m_client = new QMqttClient(this);
    connect(m_client, &QMqttClient::stateChanged, this, &InputMqtt::onStateChanged);
    connect(m_client, &QMqttClient::messageReceived, this, &InputMqtt::onMessageReceived);
    connect(m_autoReconnectTimer, &QTimer::timeout, this, &InputMqtt::reconnect);
    setObjectName("InputMqtt");
}

InputMqtt::~InputMqtt()
{
    delete m_client;
}

bool InputMqtt::validateUrl(const QUrl &url)
{
    return url.isEmpty() || (url.isValid() && (url.scheme() == "http" || url.scheme() == "https"));
}

void InputMqtt::reconnect()
{
    m_autoReconnectInProgress = true;
    m_client->connectToHost();
}

void InputMqtt::disconnect()
{
    m_client->disconnectFromHost();
}

void InputMqtt::subscribeToTopic()
{
    auto subOk = m_client->subscribe(m_topic, m_qos);
    if(m_client->state() == QMqttClient::Connected && !subOk) {
        emit statusToLog("Could not subscribe. Is there a valid connection?");
    } else if(subOk) {
        emit statusToLog("Subscribed to topic: " + m_topic);
    }
}

void InputMqtt::setBroker(const QUrl &addr)
{
    m_autoReconnectTimer->stop();

    if(m_client->state() == QMqttClient::Connected && addr.host() == m_brokerHostname && addr.port() == m_brokerPort) return;

    m_brokerHostname = addr.host();
    m_brokerPort = addr.port();

    m_client->disconnectFromHost();

    if(!addr.isEmpty() && validateUrl(addr)) {
        m_client->setHostname(m_brokerHostname);
        m_client->setPort(m_brokerPort);
        m_client->connectToHost();
    } else if(!validateUrl(addr)) {
        emit statusToLog("Invalid broker address!");
    }
}

void InputMqtt::setPassword(const QString &pw)
{
    if(pw == m_password) return;

    m_password = pw;
    m_client->setPassword(m_password);
}

void InputMqtt::setUsername(const QString &user)
{
    if(user == m_username) return;

    m_username = user;
    m_client->setUsername(m_username);
}

void InputMqtt::setQos(int qos)
{
    if(qos == m_qos) return;

    m_qos = qos;
}

void InputMqtt::setTopic(const QString &topic)
{
    if(topic == m_topic) return;

    if(!m_topic.isEmpty()) {
        m_client->unsubscribe(m_topic);
        emit statusToLog("Unsubscribed from topic: " + m_topic);
    }
    m_topic = topic;
    subscribeToTopic();
}

void InputMqtt::onStateChanged()
{
    if(m_client->state() == QMqttClient::Disconnected) {
        if(!m_brokerHostname.isEmpty()) {
            m_autoReconnectTimer->setInterval(10000);
            m_autoReconnectTimer->setSingleShot(true);
            m_autoReconnectTimer->start();
        } else {
            qDebug() << "MQTT autoreconnecting...";
        }
        if(!m_autoReconnectInProgress) {
             emit statusToLog("<font color='red'>Broker disconnected!</font>");
        }
    } else if(m_client->state() == QMqttClient::Connecting) {
        if(!m_autoReconnectInProgress) {
            emit statusToLog("Connecting...");
        }
    } else if(m_client->state() == QMqttClient::Connected) {
        m_autoReconnectInProgress = false;
        emit statusToLog("Broker connected!");
        subscribeToTopic();
    }
}

void InputMqtt::setClientPort(int p)
{
    m_client->setPort(p);
}

void InputMqtt::onPublish(const QString &msg, const QString &subtopic)
{
    m_client->publish(QMqttTopicName(QString("%1/%2").arg(m_topic).arg(subtopic)), msg.toUtf8(), m_qos);
}

void InputMqtt::onMessageReceived(const QString &msg)
{
    emit messageToEndpoint(msg);
}

