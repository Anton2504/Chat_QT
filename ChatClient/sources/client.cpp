#include "client.h"


Client::Client(QObject *parent)
    : QObject{parent}   {

    socket = new QTcpSocket(this);

    connect(socket, &QTcpSocket::disconnected, this, &Client::onConnectionLost);
    connect(socket, &QTcpSocket::readyRead, this, &Client::onGetData);

    socket->connectToHost("127.0.0.1", 55120);
    nextBlockSize =0;
    if(!socket->waitForConnected())    {
        onConnectionLost();
    }
}

QTcpSocket *Client::getSocket() const   {
    return socket;
}

void Client::sendToServer(const QJsonObject &json){

    QJsonDocument docJson(json);
    QByteArray jsonData = docJson.toJson();
    socket->waitForBytesWritten();
    Data.clear();
    QDataStream out(&Data, QIODevice::WriteOnly);
    out.setVersion(QDataStream::Qt_6_9);
    out << quint16(0) << jsonData;
    out.device()->seek(0);
    out << quint16(Data.size() - sizeof(quint16));
    socket->write(Data);
}

void Client::onGetData()    {

    QDataStream in(socket);
    in.setVersion(QDataStream::Qt_6_9);
    if(in.status() == QDataStream::Ok)    {
        for(;;)        {
            if(nextBlockSize == 0)            {
                if(socket->bytesAvailable() < 2)                {
                    break;
                }
                in >> nextBlockSize;
            }
            if(socket->bytesAvailable() < nextBlockSize)            {
                break;
            }
            in >> package;
            responseHandler(package);
            nextBlockSize = 0;
        }
    }
    else    {
        QMessageBox::critical(nullptr, tr("Error"), tr("Error receive data"));
    }
}

const QVector<QString>& Client::getAllUsers() const{
    return _users;
}

void Client::onConnectionLost() {
    QMessageBox::critical(nullptr, "Error", "Failed connect to server!");
    exit(0);
}

void Client::responseHandler(QJsonObject &json) {

    if(json["cmd"].toString() == "add_user"){
        emit addUserJson(json);
    }
    else if(json["cmd"].toString() == "reg_user"){
        emit regResponceJson(json);
    }
    else if(json["cmd"].toString() == "auth_user"){
        emit authResponceJson(json);
    }
    else if(json["cmd"].toString() == "upd_users"){
        usersFromServer(json);
    }
    else if(json["cmd"].toString() == "all_msgs"){
        allMessagesFromServer(json);
    }
    else if(json["cmd"].toString() == "client_msgs"){
        privateMsgsFromServer(json);
    }
}

void Client::usersFromServer(const QJsonObject &json){
    const QJsonArray allUsers = json.value("users").toArray();
    QVector<QString> tempUsrs;
    tempUsrs.reserve(allUsers.size());
    for (const QJsonValue &value : allUsers){
        tempUsrs.push_back(value.toString());
    }
    _users.swap(tempUsrs);
}

void Client::allMessagesFromServer(const QJsonObject &json){
    const QJsonArray allMsgs = json.value("all_msgs").toArray();
    QVector<QString> tempMsgs;
    tempMsgs.reserve(allMsgs.size());
    for (const QJsonValue &value : allMsgs){
        tempMsgs.push_back(value.toString());
    }
    _allMsgs.swap(tempMsgs);
}

void Client::privateMsgsFromServer(const QJsonObject& json){
    const QJsonArray PrivateMsgs = json.value("pm_msgs").toArray();
    QVector<QString> tempMsgs;
    tempMsgs.reserve(PrivateMsgs.size());
    for (const QJsonValue &value : PrivateMsgs){
        tempMsgs.push_back(value.toString());
    }
    _privateMsgs.swap(tempMsgs);
}

const QVector<QString>& Client::getPrivateMsgs() const {
    return _privateMsgs;
}

const QVector<QString>& Client::getAllMsgs() const{
    return _allMsgs;
}

void Client::updPrivateClientMessages(const QString login){
    QJsonObject req;
    req["cmd"] = "get_private_msg";
    req["login"] = login;
    sendToServer(req);
}
