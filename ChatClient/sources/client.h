#ifndef CLIENT_H
#define CLIENT_H

#include <QFile>
#include <QObject>
#include <QString>
#include <QTcpSocket>
#include <QMessageBox>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>


class Client : public QObject
{
    Q_OBJECT
public:
    explicit Client(QObject *parent = nullptr);
    void sendToServer(const QJsonObject &json);
    QTcpSocket *getSocket() const;
    void usersFromServer(const QJsonObject &json);
    void allMessagesFromServer(const QJsonObject &json);
    void privateMsgsFromServer(const QJsonObject& json);
    const QVector<QString>& getPrivateMsgs() const;
    const QVector<QString>& getAllMsgs() const;
    const QVector<QString>& getAllUsers() const;


signals:
    void connected();
    void disconnected();
    void addUserJson(QJsonObject& usrPkg);
    void authResponceJson(QJsonObject authPkg);
    void regResponceJson(QJsonObject regPkg);

public slots:
    void updPrivateClientMessages(const QString login);
    void onGetData();
    void onConnectionLost();

private:
    void responseHandler(QJsonObject &json);

private:
    QVector<QString> _allMsgs;
    QVector<QString> _privateMsgs;
    QVector<QString> _users;
    QTcpSocket * socket;
    QByteArray Data;
    quint16 nextBlockSize;
    QJsonObject package;
};

#endif

