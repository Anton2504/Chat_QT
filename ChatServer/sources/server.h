#ifndef SERVER_H
#define SERVER_H
#include <QTcpServer>
#include <QTcpSocket>
#include <QJsonDocument>
#include <QJsonObject>
#include "mysql.h"

class Net : public QTcpServer
{
    Q_OBJECT

public:
    Net();
    ~Net();
    void startListen();
    QVector<QTcpSocket *>& getSockets();
    Database* getDatabase() const;

private:
    void preparePackage();
    void sendRequest(const QJsonObject& reqName);
    void sendPersonalRequest(const QJsonObject& reqName, QTcpSocket* cliSocket);
    void requestHandler(QTcpSocket* socket);
    void authClient(QTcpSocket* cliSocket);
    void regClient(QTcpSocket* cliSocket);
    void sendMsgToSqlDb(const QString& msg_to);
    void getPrivateClientMsg(const QString login, QTcpSocket* cliSocket);

private slots:
    void incomingConnection(qintptr socketDescriptor);
    void slotClientDisconnect();
    void slotReadyRead();
public slots:
    void updAllMessages();
    void updUsers();

private:
    QTcpSocket *socket;
    QVector <QTcpSocket*> Sockets;
    QByteArray Data;
    quint16 nextBlockSize;
    quint16 chatPort;
    QJsonObject requestJson, packageJson;
    Database* mysql;

};
#endif // SERVER_H

