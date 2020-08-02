#ifndef INPUTMQTT_H
#define INPUTMQTT_H

#include <QMqttClient>
#include <QUrl>
#include <QTimer>

class InputMqtt : public QObject
{
    Q_OBJECT

signals:
    void messageToScreen(QString msg);
    void statusToLog(QString msg);

public:
    InputMqtt();
    bool validateUrl(const QUrl &url);
    void disconnect();
    void subscribeToTopic();

    //Setters
    void setBroker(const QUrl &addr);
    void setPassword(const QString &pw);
    void setUsername(const QString &user);
    void setQos(int qos);
    void setTopic(const QString &topic);

    //Getters
    QUrl getBroker() {
        QUrl broker;
        broker.setHost(m_brokerHostname);
        broker.setPort(m_brokerPort);
        return broker;
    }
    QString getUsername() { return m_username; }
    QString getPassword() { return m_password; }
    QString getTopic() { return m_topic; }
    int getQos() { return m_qos; }


public slots:
    void setClientPort(int p);
    void onPublish(QString msg);
    void reconnect();

private slots:
    void onStateChanged();
    void onMessageReceived(QString msg);

private:
    QMqttClient *m_client;
    QString m_brokerHostname;
    int m_brokerPort;
    QString m_username;
    QString m_password;
    int m_qos;
    QString m_topic;
    QTimer *m_autoReconnectTimer = new QTimer(this);
    bool m_autoReconnectInProgress = false;
};

#endif // INPUTMQTT_H
