#include "mysql.h"
#include <QSqlError>

Database::~Database(){
    if(db.open())
        db.close();
}

const QHash<QString,QByteArray>& Database::getUSers() const{
    return _users;
}

const bool& Database::getAllMsgsFlagValid() const{
    return allMsgs_is_valid;
}

void Database::setAllMsgsFlagValid(){
    allMsgs_is_valid = true;
}

void Database::setUsersFlagValid(){
    users_list_is_valid = true;
}

const bool& Database::getUsersFlagValid() const{
    return users_list_is_valid;
}

bool Database::getUserPmMsgsFalagValid(const QString& login) const{
    return users_pmMsgs_valid.value(login);
}

void Database::setUserPmMsgsFalagValid(const QString& login){
    users_pmMsgs_valid[login] = true;
}

bool Database::blockUser(const QString& login){
    QSqlQuery query;
    query.prepare("UPDATE users SET status = 0x02 "
                  "where login = :login");
    query.bindValue(":login", login);
    if(!query.exec()){
        qDebug() << "Error select:" << query.lastError().text();
        return false;
    }else{
        return true;
    }
}

bool Database::unblockUser(const QString& login){
    QSqlQuery query;
    query.prepare("UPDATE users SET status = 0x01 "
                  "where login = :login");
    query.bindValue(":login", login);
    if(!query.exec()){
        qDebug() << "Error select:" << query.lastError().text();
        return false;
    }else{
        return true;
    }
}

bool Database::getAllMsgsQuery(){

    QJsonObject allMsgs;
    QSqlQuery query;
    QDateTime datetime;
    QString formattedDateTime;
    query.prepare("select datetime, login, text from users join chat where user_uuid=uuid ORDER by datetime");
    if(!query.exec()){
        qDebug() << "Error select login, uuid from users:" << query.lastError().text();
        return false;
    }else{
        QJsonArray msgs;
        while(query.next()){
            datetime = query.value("datetime").toDateTime();
            formattedDateTime = datetime.toString("yyyy-MM-dd hh:mm:ss");
            msgs.append("["+formattedDateTime+"] <"+
                        query.value("login").toString()+"> пишет: "+
                        query.value("text").toString());
        }
        allMsgs.insert("all_msgs",msgs);
        allMsgs.swap(_allMsg);
        //setAllMsgsFlagValid();
        return true;
    }
}

QJsonObject Database::getPrivateMsgsQuery(const QString& login){
    QJsonObject pmMsgs;
    QSqlQuery query;
    QDateTime datetime;
    QString formattedDateTime;
    query.prepare("select login, text, dateime from "
                  "(select uuid_sender, uuid_recipient, text, dateime from messages "
                  "join users where uuid = uuid_recipient and login = :login) "
                  "as t join users where uuid_sender  = uuid "
                  "ORDER by dateime");
    query.bindValue(":login", login);
    if(!query.exec()){
        qDebug() << "Error select:" << query.lastError().text();
        return pmMsgs;
    }else{
        QJsonArray msgs;
        while(query.next()){
            datetime = query.value("dateime").toDateTime();
            formattedDateTime = datetime.toString("yyyy-MM-dd hh:mm:ss");
            msgs.append("["+formattedDateTime+"] <"+
                        query.value("login").toString()+"> пишет: "+
                        query.value("text").toString());
        }
        pmMsgs.insert("pm_msgs",msgs);
        return pmMsgs;
    }
}

const QJsonObject& Database::getAllMsgs() const{
    return _allMsg;
}

bool Database::getDbUSers(){

    QSqlQuery query;
    QHash<QString,QByteArray> tempUsrs;
    QHash<QString,bool> tempUsrsPmMsgs;
    query.prepare("select login, uuid from users where status = 0x01");//SELECT COUNT(*) FROM users
    if(!query.exec()){
        qDebug() << "Error select login, uuid from users:" << query.lastError().text();
        return false;
    }else{
        tempUsrs.reserve(query.size());
        tempUsrsPmMsgs.reserve(query.size());
        while(query.next()){
            tempUsrs.insert(query.value("login").toString(),query.value("uuid").toByteArray());
            tempUsrsPmMsgs.insert(query.value("login").toString(),false);
        }
        tempUsrs.swap(_users);
        tempUsrsPmMsgs.swap(users_pmMsgs_valid);
        return true;
    }
}

bool Database::getUsersBlocked(){

    QSqlQuery query;
    QHash<QString,QByteArray> tempUsrs;
    query.prepare("select login, uuid from users where status = 0x02");
    if(!query.exec()){
        qDebug() << "Error select login, uuid from users:" << query.lastError().text();
        return false;
    }else{
        tempUsrs.reserve(query.size());
        while(query.next()){
            tempUsrs.insert(query.value("login").toString(),query.value("uuid").toByteArray());
        }
        tempUsrs.swap(_usersBlocked);
        return true;
    }
}

const QHash<QString,QByteArray>& Database::getBlockUSers() const{
    return _usersBlocked;
}

bool Database::newAllMsg(const QString& login, const QString& msg){

    getDbUSers();
    QSqlQuery query;
    QUuid uuidMsg = QUuid::createUuidV7();
    query.prepare("INSERT INTO chat (uuid_chatmsg, user_uuid, text, dateime) "
                  "VALUES (UUID_TO_BIN(:uuid_msg), :uuid_sender, :msg, NOW())");
    query.bindValue(":uuid_msg", uuidMsg);
    query.bindValue(":uuid_sender", _users.value(login));
    query.bindValue(":msg", msg);
    if(!query.exec()){
        qDebug() << "Error inserting record:" << query.lastError().text();
        return false;
    }else{
        allMsgs_is_valid = false;
        //setAllMsgsFlagValid();
        return true;
    }
}

bool Database::newPrivateMsg(const QString& sender, const QString& recipient, const QString& msg){

    getDbUSers();
    QSqlQuery query;
    QUuid uuidMsg = QUuid::createUuidV7();
    query.prepare("INSERT INTO messages (uuid_msg, uuid_sender, uuid_recipient, text, dateime) "
                  "VALUES (UUID_TO_BIN(:uuid_msg), :uuid_sender, :uuid_recipient, :msg, NOW())");
    query.bindValue(":uuid_msg", uuidMsg);
    query.bindValue(":uuid_sender", _users.value(sender));
    query.bindValue(":uuid_recipient", _users.value(recipient));
    query.bindValue(":msg", msg);
    if(!query.exec()){
        qDebug() << "Error inserting record:" << query.lastError().text();
        return false;
    }else{
        users_pmMsgs_valid[recipient] = false;
        return true;
    }
}

QJsonObject Database::getUsersToServer() {
    QJsonObject users;
    QJsonArray usersArr;
    if(getDbUSers()){
        const QList usersList = _users.keys();
        for(auto const& user : usersList){
            QJsonObject userJson;
            userJson["login"] = user;
            usersArr.append(userJson);
        }
    }
    users["users"] = usersArr;
    return users;
}

bool Database::authLogin(const QString& login, const QString& pass){

    if(!chekLogin(login) && chekPass(login,pass)){
        return true;
    }
    return false;
}

bool Database::regLogin(const QString& login, const QString& pass){

    if(!chekLogin(login)){
        QByteArray status_newUser(1, '\x01');
        QUuid uuid = QUuid::createUuidV7();
        QSqlQuery query;
        query.prepare("INSERT INTO users (uuid, login, status) VALUES (UUID_TO_BIN(:uuid), :login, :status)");
        query.bindValue(":uuid", uuid);
        query.bindValue(":login", login);
        query.bindValue(":status", status_newUser);
        if(!query.exec()){
            qDebug() << "Error inserting record:" << query.lastError().text();
            return false;
        }else{
            QByteArray pwd = QByteArray::fromHex(pass.toLatin1());
            query.prepare("UPDATE users_login SET pwd = :pwd where user_uuid = UUID_TO_BIN(:uuid)");
            query.bindValue(":pwd", pwd);
            query.bindValue(":uuid", uuid);
            if(!query.exec()){
                qDebug() << "Error inserting record:" << query.lastError().text();
                query.prepare("DELETE FROM users where login = :login");
                query.bindValue(":login", login);
                query.exec();
                return false;
            }
            return true;
        }
    }
    qDebug() << "db.close()";
    return false;
}

bool Database::chekLogin(const QString& login)  {

    QSqlQuery query;
    query.exec("select login from users where login = :login ");/*and status = 0x01*/
    query.bindValue(":login", login);
    return query.next()?true:false;
}

bool Database::chekPass(const QString& login, const QString& pass)  {

    QSqlQuery query;
    query.prepare("select pwd from users join users_login where uuid = user_uuid and login = :login");
    query.bindValue(":login", login);
    if(!query.exec()){
        qDebug() << "Error:" << query.lastError().text();
    }else{
        if(query.next() && query.value("pwd").toByteArray() == QByteArray::fromHex(pass.toLatin1())){
            return true;
        }
    }
    return false;
}

void Database::runDB(){

    QSqlQuery query;
    query.exec("CREATE DATABASE IF NOT EXISTS Chat_db");

    query.exec("USE Chat_db");

    query.exec("CREATE TABLE IF NOT EXISTS users ("
               "uuid BINARY(16) NOT NULL PRIMARY KEY,"
               " login VARCHAR(20) NOT NULL UNIQUE KEY,"
               " status BINARY(1) NOT NULL)");

    query.exec("CREATE TABLE IF NOT EXISTS `users_login` ("
               "`user_uuid` BINARY(16) NOT NULL UNIQUE KEY,"
               "`pwd` binary(32) DEFAULT NULL,"
               "CONSTRAINT `pass_FK` FOREIGN KEY (`user_uuid`) "
               "REFERENCES `users` (`uuid`) ON DELETE CASCADE ON UPDATE CASCADE"
               ") ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_0900_ai_ci;");


    query.exec("CREATE TABLE IF NOT EXISTS `messages` ("
               "`uuid_msg` binary(16) NOT NULL,"
               "`text` varchar(1024),"
               "`uuid_sender` binary(16) NOT NULL,"
               "`uuid_recipient` binary(16) NOT NULL,"
               "`status_msg` tinyint(1) DEFAULT NULL,"
               "`dateime` datetime NOT NULL,"
               "PRIMARY KEY (`uuid_msg`),"
               "KEY `messages_users_FK` (`uuid_sender`),"
               "CONSTRAINT `messages_users_FK` FOREIGN KEY (`uuid_sender`) "
               "REFERENCES `users` (`uuid`) ON DELETE CASCADE ON UPDATE CASCADE"
               ") ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_0900_ai_ci;");

    query.exec("CREATE TABLE IF NOT EXISTS `chat` ("
               "`uuid_chatmsg` binary(16) NOT NULL,"
               "`text` varchar(255),"
               "`user_uuid` binary(16) NOT NULL,"
               "`datetime` datetime NOT NULL,"
               "PRIMARY KEY (`uuid_chatmsg`),"
               "KEY `chat_ibfk_1` (`user_uuid`),"
               "CONSTRAINT `chat_ibfk_1` FOREIGN KEY (`user_uuid`) "
               "REFERENCES `users` (`uuid`) ON DELETE CASCADE ON UPDATE CASCADE"
               ") ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_0900_ai_ci;");
    query.exec("CREATE TRIGGER IF NOT EXISTS upd_user "
               " AFTER INSERT ON users FOR EACH ROW begin "
               " insert into users_login (user_uuid, pwd) values (new.uuid,Null); "
               " end;");

    QByteArray status_Admin(1, '\x28');
    QUuid uuid = QUuid::createUuidV7();
    query.prepare("INSERT INTO users (uuid, login, status) VALUES (UUID_TO_BIN(:uuid), :login, :status)");
    query.bindValue(":uuid", uuid);
    query.bindValue(":login", "Admin");
    query.bindValue(":status", status_Admin);
    if(!query.exec()){
        //qDebug() << "Error inserting record:" << query.lastError().text();
    }else{
        QString pass = "c1c224b03cd9bc7b6a86d77f5dace40191766c485cd55dc48caf9ac873335d6f";
        QByteArray pwd = QByteArray::fromHex(pass.toLatin1());
        query.prepare("UPDATE users_login SET pwd = :pwd where user_uuid = UUID_TO_BIN(:uuid)");
        query.bindValue(":pwd", pwd);
        query.bindValue(":uuid", uuid);
        if(!query.exec()){
            qDebug() << "Error inserting record:" << query.lastError().text();
            query.prepare("DELETE FROM users where login = :login");
            query.bindValue(":login", "Admin");
            query.exec();
        }
    }
    //db.close();
}

