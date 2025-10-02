#ifndef CLIENT_H
#define CLIENT_H

#include <QFile>
#include <QObject>
#include <QString>
#include <QTcpSocket>
#include <QMessageBox>
//#include <QRegularExpression>
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
    //void addUser(QJsonObject& usrPkg);
    //void authResponce(QJsonObject authPkg);
    //void regResponce(QJsonObject regPkg);
    void addUserJson(QJsonObject& usrPkg);
    void authResponceJson(QJsonObject authPkg);
    void regResponceJson(QJsonObject regPkg);
    // void updateUsrBase();
    // void updateAMBase();
    // void updatePMBase();
    // void addAllMsg(QJsonObject& msgPkg);
    // void addPerMsg(QJsonObject& msgPkg);
    // void addAllMsgJson(QJsonObject& msgPkg);
    // void addPerMsgJson(QJsonObject& msgPkg);

public slots:
    void updPrivateClientMessages(const QString login);
    //void sendMsgAll(const QJsonObject& msg);
    void onGetData();
    void onConnectionLost();
    // void sendAMStatus(const QJsonObject& pkg);
    // void sendPMStatus(const QJsonObject& pkg);

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
