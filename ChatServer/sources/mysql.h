#ifndef MYSQL_H
#define MYSQL_H
//#include <QDebug>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QByteArray>
#include <QJsonArray>
#include <QJsonObject>
#include <QList>
#include <QUuid>
#include <QHash>
#include <QDateTime>


class Database : public QObject{
    Q_OBJECT
public:
    Database() = default;
    ~Database();
    void connect()
    {
        db = QSqlDatabase::addDatabase("QMYSQL");
        db.setHostName("localhost");
        db.setUserName("root");
        db.setPassword("Vthrekmtd@$7753191");
        db.open();
        runDB();
    }
    bool chekLogin(const QString& login);
    bool chekPass(const QString& login, const QString& pass);
    bool regLogin(const QString& login, const QString& pass);
    bool authLogin(const QString& login, const QString& pass);
    bool getDbUSers();
    bool getUsersBlocked();
    bool newAllMsg(const QString& login, const QString& msg);
    bool newPrivateMsg(const QString& sender, const QString& recipient, const QString& msg);
    bool getAllMsgsQuery();
    QJsonObject getPrivateMsgsQuery(const QString& login);
    const QJsonObject& getAllMsgs() const;
    const QHash<QString,QByteArray>& getUSers() const;
    const QHash<QString,QByteArray>& getBlockUSers() const;
    void setUserPmMsgsFalagValid(const QString& login);
    void setAllMsgsFlagValid();
    void setUsersFlagValid();
    const bool& getAllMsgsFlagValid() const;
    const bool& getUsersFlagValid() const;
    bool getUserPmMsgsFalagValid(const QString& login) const;
    QJsonObject getUsersToServer();
    bool blockUser(const QString& login);
    bool unblockUser(const QString& login);

private:
    void runDB();

    QJsonObject _allMsg;
    QHash<QString,QByteArray> _users;
    QSqlDatabase db;
    bool allMsgs_is_valid = false;
    bool users_list_is_valid = false;
    QHash<QString,bool> users_pmMsgs_valid;
    QHash<QString,QByteArray> _usersBlocked;

signals:
    void logUpdated(const QString& entry);
};
#endif // MYSQL_H
