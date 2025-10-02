#include "server.h"
#include <QDebug>

Net::Net()
{
    nextBlockSize = 0;
    chatPort = 55120;

    mysql = new Database;
    mysql->connect();
    mysql->getDbUSers();
}

Net::~Net(){
    delete mysql;
}

void Net::startListen() {

    if(this->listen(QHostAddress::Any, 55120))
        qDebug() << "start listen";
    nextBlockSize = 0;
}

void Net::incomingConnection(qintptr socketDescriptor)  {

    socket = new QTcpSocket;
    socket->setSocketDescriptor(socketDescriptor);

    connect(socket, &QTcpSocket::readyRead, this, &Net::slotReadyRead);
    connect(socket, &QTcpSocket::disconnected, this, &Net::slotClientDisconnect);

    Sockets.push_back(socket);
}

void Net::slotClientDisconnect()   {

    auto client = qobject_cast<QTcpSocket*>(sender());
    Sockets.removeOne(client);
}

void Net::preparePackage()  {
    Data.clear();
    QDataStream out(&Data, QIODevice::WriteOnly);
    out.setVersion(QDataStream::Qt_6_9);
    out << quint16() << packageJson;
    out.device()->seek(0);
    out << quint16(Data.size() - sizeof(quint16));
}

void Net::sendRequest(const QJsonObject& reqName)   {
    packageJson = reqName;
    preparePackage();
    for(int i = 0; i < Sockets.size(); i++)
    {
        Sockets[i]->write(Data);
    }
}

void Net::updAllMessages(){
    if(!mysql->getAllMsgsFlagValid()){
        if(mysql->getAllMsgsQuery()){
            QJsonObject sendAllMsgs = mysql->getAllMsgs();
            sendAllMsgs["cmd"] = "all_msgs";
            sendRequest(sendAllMsgs);
            //mysql->setAllMsgsFlagValid();
        }
    }
}

void Net::updUsers(){

    if(!mysql->getUsersFlagValid()){
        QJsonObject users = mysql->getUsersToServer();
        users["cmd"] = "upd_users";
        if(!users.empty()){
            users["status"] = true;
            users["users"] = users["users"];
        }else{
            users["status"] = false;
        }
        sendRequest(users);
        //mysql->setUsersFlagValid();
    }
}

void Net::sendPersonalRequest(const QJsonObject& reqName, QTcpSocket* sock) {
    packageJson = reqName;
    preparePackage();
    sock->write(Data);
}

void Net::slotReadyRead()   {

    socket = (QTcpSocket*)sender();
    QDataStream in(socket);
    in.setVersion(QDataStream::Qt_6_9);
    if(in.status() == QDataStream::Ok)  {
        for(;;) {
            if(nextBlockSize == 0)  {
                if(socket->bytesAvailable() < 2)    {
                    break;
                }
                in >> nextBlockSize;
            }
            if(socket->bytesAvailable() < nextBlockSize)    {
                break;
            }
            in >> requestJson;
            requestHandler(socket);
            nextBlockSize = 0;
            break;
        }
    }
}

void Net::requestHandler(QTcpSocket* cliSocket) {

    if(requestJson["cmd"].toString() == "auth_user"){
        authClient(cliSocket);
    }    
    else if(requestJson["cmd"].toString() == "reg_user"){
        regClient(cliSocket);
    }
    else if(requestJson["cmd"].toString() == "msg_all"){
        sendMsgToSqlDb("all");
    }
    else if(requestJson["cmd"].toString() == "msg_private"){
        sendMsgToSqlDb("private");
    }
    else if(requestJson["cmd"].toString() == "get_private_msg"){
        getPrivateClientMsg(requestJson["login"].toString(), cliSocket);
    }
}

void Net::authClient(QTcpSocket* cliSocket) {

    QJsonObject req;
    if(mysql->authLogin(requestJson["login"].toString(),
                         requestJson["pass"].toString())){

        req["cmd"] = "auth_user";
        req["status"] = true;
        req["login"] = requestJson["login"].toString();
        req["info"] = "Authentification success for client " +
                      QString::number(socket->socketDescriptor());
        sendPersonalRequest(req, cliSocket);
    }    else    {
        req["cmd"] = "auth_user";
        req["status"] = false;
        req["login"] = "";
        req["info"] = "Authentification failed for client " +
                      QString::number(socket->socketDescriptor());
        sendPersonalRequest(req, cliSocket);
    }
}

void Net::regClient(QTcpSocket* cliSocket)  {

    QJsonObject req;
    if(mysql->regLogin(requestJson["login"].toString(),
                         requestJson["pass"].toString()))    {
            req["cmd"] = "reg_user";
            req["status"] = true;
            req["login"] = requestJson["login"].toString();
            req["info"] = "Registration completed for client " +
                          QString::number(socket->socketDescriptor());
            sendPersonalRequest(req, cliSocket);
    }    else    {
        req["cmd"] = "reg_user";
        req["status"] = false;
        req["login"] = "";
        req["info"] = "Registration failed for client " +
                      QString::number(socket->socketDescriptor());
        sendPersonalRequest(req, cliSocket);
    }
}

void Net::getPrivateClientMsg(const QString login, QTcpSocket* cliSocket){
    if(!mysql->getUserPmMsgsFalagValid(login)){
        QJsonObject req = mysql->getPrivateMsgsQuery(requestJson["login"].toString());
        req["cmd"] = "client_msgs";
        sendPersonalRequest(req,cliSocket);
        //mysql->setUserPmMsgsFalagValid(login);
    }
}

QVector<QTcpSocket *>& Net::getSockets()    {
    return Sockets;
}

Database* Net::getDatabase() const{
    return mysql;
}

void Net::sendMsgToSqlDb(const QString &msg_to)   {
    if(msg_to == "all"){
        mysql->newAllMsg(requestJson["name"].toString(),
                         requestJson["text"].toString());
    }
    else if(msg_to == "private"){
        mysql->newPrivateMsg(requestJson["sender"].toString(),
                         requestJson["recipient"].toString(),
                         requestJson["text"].toString());
    }
}
