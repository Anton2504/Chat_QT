#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "startscreen.h"
#include <QDialog>
#include <QVBoxLayout>
#include <QDialogButtonBox>
#include <QListWidget>
#include <QTimer>
#include <QStringListModel>

MainWindow::MainWindow(QString userName, Net* server, QWidget *parent):
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    _server(server),
    _userName(userName)
{
    ui->setupUi(this);
    _server = server;
    initConnections();
    auto timerServer = new QTimer(this);
    auto timerClient = new QTimer(this);



    connect(timerServer, &QTimer::timeout, this, &MainWindow::updAllMessages);
    connect(timerServer, &QTimer::timeout, _server, &Net::updAllMessages);
    connect(timerClient, &QTimer::timeout, _server, &Net::updUsers);

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
    auto w = new MainWindow(s.userName(),s.getServer());
    w->setAttribute(Qt::WA_DeleteOnClose);
    return w;
}

 void MainWindow::updAllMessages(){

     bool result = false;

     if(!_server->getDatabase()->getAllMsgsFlagValid()){
         result = _server->getDatabase()->getAllMsgsQuery();
     }

     if(result){
         auto chatMessages = _server->getDatabase()->getAllMsgs();
         auto chatMsgs = chatMessages["all_msgs"].toArray();
         QString chat;
         for(qsizetype i=0;i != chatMsgs.size();++i){
             chat.append(chatMsgs[i].toString() + "\n");
         }
         if(ui->commonChatBrowser->toPlainText() != chat)
             ui->commonChatBrowser->setText(chat);
     }
}

void MainWindow::initConnections()
{
    //connect(ui->sendMessageButton,&QPushButton::clicked,this,&MainWindow::sendMsgAll);
    //connect(ui->privateMessageSendButton,&QPushButton::clicked,this,&MainWindow::sendMsgPrivate);
    connect(ui->BlockUser,&QPushButton::clicked,this,&MainWindow::blockUser);
    connect(ui->UnblockUser,&QPushButton::clicked,this,&MainWindow::unblockUser);

    connect(ui->actionCloseClient,&QAction::triggered,this,&MainWindow::close);


}

void MainWindow::blockUser(){

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

    const QList usersList = _server->getDatabase()->getUSers().keys();
    for(const auto &user : usersList)
    {
        userListWgt->addItem(user);
    }

    userListWgt->setCurrentRow(0);

    auto result = users.exec();

    if(result == QDialog::Accepted &&
        userListWgt->currentItem())
    {
        _server->getDatabase()->blockUser(userListWgt->currentItem()->text());
    }
}

void MainWindow::unblockUser(){

    QDialog usersBlocked(this);
    usersBlocked.setModal(true);
    auto l = new QVBoxLayout(&usersBlocked);
    usersBlocked.setLayout(l);
    auto userListWgt = new QListWidget(&usersBlocked);
    l->addWidget(userListWgt);
    auto buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, &usersBlocked);
    l->addWidget(buttonBox);

    connect(buttonBox, &QDialogButtonBox::accepted, &usersBlocked, &QDialog::accept);
    connect(buttonBox, &QDialogButtonBox::rejected, &usersBlocked, &QDialog::reject);

    _server->getDatabase()->getUsersBlocked();

    const QList usersList = _server->getDatabase()->getBlockUSers().keys();

    for(const auto &user : usersList)
    {
        userListWgt->addItem(user);
    }

    userListWgt->setCurrentRow(0);

    auto result = usersBlocked.exec();

    if(result == QDialog::Accepted &&
        userListWgt->currentItem())
    {
        _server->getDatabase()->unblockUser(userListWgt->currentItem()->text());
    }
}
