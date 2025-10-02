#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "startscreen.h"
#include <QDialog>
#include <QVBoxLayout>
#include <QDialogButtonBox>
#include <QListWidget>
#include <QTimer>
#include <QStringListModel>

MainWindow::MainWindow(QString userName, Client* client,/* Net* server,*/ QWidget *parent):
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    _client(client),
    _userName(userName)
{
    ui->setupUi(this);
    _client = client;
    initConnections();
    auto timerServer = new QTimer(this);
    auto timerClient = new QTimer(this);

    connect(timerServer, &QTimer::timeout, this, &MainWindow::updAllMessages);

    connect(timerClient, &QTimer::timeout, this, &MainWindow::updPrivateMessages);
    connect(this,&MainWindow::updPrivateMsgsFromServer,_client,&Client::updPrivateClientMessages);

            timerServer->start(1000);
            timerClient->start(1333);
}

MainWindow::~MainWindow()
{
    delete ui;
}

MainWindow *MainWindow::createClient()
{
    StartScreen s;
    auto result = s.exec();
    if(result == QDialog::Rejected)
    {
        return nullptr;
    }
    auto w = new MainWindow(s.userName(), s.getSocket()/*,s.getServer()*/);
    w->setAttribute(Qt::WA_DeleteOnClose);
    return w;
}

void MainWindow::updPrivateMessages(){
    emit updPrivateMsgsFromServer(_userName);

    auto privateMessages = _client->getPrivateMsgs();
    QString chat;
    for(const auto &msg : privateMessages)
    {
        chat.append(msg + "\n");
    }
    if(ui->privateChatBrowser->toPlainText() != chat)
        ui->privateChatBrowser->setText(chat);
}

void MainWindow::updAllMessages(){

    auto allMessages = _client->getAllMsgs();
    QString chat;
    for(const auto &msg : allMessages)
    {
        chat.append(msg + "\n");
    }

    if(ui->commonChatBrowser->toPlainText() != chat)
        ui->commonChatBrowser->setText(chat);
}

void MainWindow::sendMsgAll(){
    QString msg = ui->messageLineEdit->text();
    if(!msg.isEmpty()){
        QJsonObject msgJson;
        msgJson["cmd"] = "msg_all";
        msgJson["name"] = _userName;
        msgJson["text"] = msg;
        _client->sendToServer(msgJson);
        ui->messageLineEdit->clear();
    }
}

void MainWindow::sendMsgPrivate(){
    QDialog dial(this);
    dial.setModal(true);
    auto l = new QVBoxLayout(&dial);
    dial.setLayout(l);
    auto userListWgt = new QListWidget(&dial);
    l->addWidget(userListWgt);
    auto buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, &dial);
    l->addWidget(buttonBox);

    connect(buttonBox, &QDialogButtonBox::accepted, &dial, &QDialog::accept);
    connect(buttonBox, &QDialogButtonBox::rejected, &dial, &QDialog::reject);

    auto usersList = _client->getAllUsers();

    for(const auto &user : usersList)
    {
        userListWgt->addItem(user);
    }

    userListWgt->setCurrentRow(0);

    auto result = dial.exec();

    if(result == QDialog::Accepted &&
        userListWgt->currentItem())
    {
        QString msg = ui->messageLineEdit->text();
        if(!msg.isEmpty()){
            QJsonObject msgJson;
            msgJson["cmd"] = "msg_private";
            msgJson["sender"] = _userName;
            msgJson["recipient"] = userListWgt->currentItem()->text();
            msgJson["text"] = msg;
            _client->sendToServer(msgJson);
            ui->messageLineEdit->clear();
        }
    }    
}

void MainWindow::initConnections()
{
    connect(ui->sendMessageButton,&QPushButton::clicked,this,&MainWindow::sendMsgAll);
    connect(ui->privateMessageSendButton,&QPushButton::clicked,this,&MainWindow::sendMsgPrivate);
    connect(ui->Users,&QPushButton::clicked,this,&MainWindow::showUsers);

    connect(ui->actionOpen_another_cliend,&QAction::triggered,this,&MainWindow::createClient);
    connect(ui->actionCloseClient,&QAction::triggered,this,&MainWindow::close);


}

void MainWindow::showUsers(){

    QDialog users(this);
    users.setModal(true);
    auto l = new QVBoxLayout(&users);
    users.setLayout(l);
    auto userListWgt = new QListWidget(&users);
    l->addWidget(userListWgt);
    auto buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, &users);
    l->addWidget(buttonBox);

    connect(buttonBox, &QDialogButtonBox::accepted, &users, &QDialog::accept);
    connect(buttonBox, &QDialogButtonBox::rejected, &users, &QDialog::reject);

    userListWgt->setCurrentRow(0);

    auto result = users.exec();

    if(result == QDialog::Accepted &&
        userListWgt->currentItem())
    {
        //
    }
}
