#include "startscreen.h"
#include "ui_startscreen.h"



StartScreen::StartScreen(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::StartScreen)   {

    ui->setupUi(this);
    _server = new Net;
    _server->startListen();
    initConnections();
}

StartScreen::~StartScreen() {
    delete ui;
}

Net *StartScreen::getServer() const  {
    return _server;
}

QString StartScreen::userName() const   {
    return _userName;
}

void StartScreen::setLoginForm()    {
    ui->stackedWidget->setCurrentIndex(0);
}

void StartScreen::setRegistrationForm() {
    ui->stackedWidget->setCurrentIndex(1);
}

void StartScreen::initConnections() {

    connect(ui->loginWidget, &LoginForm::registerRequested, this, &StartScreen::setRegistrationForm);
    connect(ui->registerWidget, &RegistrationForm::loginRequested, this, &StartScreen::setLoginForm);

    connect(ui->registerWidget, &RegistrationForm::rejected, this, &StartScreen::onRejectRequested);
    connect(ui->loginWidget, &LoginForm::rejected, this, &StartScreen::onRejectRequested);

    connect(ui->registerWidget, &RegistrationForm::regUserSignal, this, &StartScreen::RegUser);

    connect(ui->loginWidget, &LoginForm::authUserSignal, this, &StartScreen::AuthUser);
}

void StartScreen::onRejectRequested(){
    reject();
}

void StartScreen::RegUser(const QString &login, const QString &pwd){
    _sha256.update(pwd);
    QString hashPass = QString::fromStdString(_sha256.backToString
                                              (_sha256.digestSTD()));
    if(_server->getDatabase()->regLogin(login,hashPass)){
        accept();
    }
}

void StartScreen::AuthUser(const QString &login, const QString &pwd)    {
    _sha256.update(pwd);
    QString hashPass = QString::fromStdString(_sha256.backToString
                                              (_sha256.digestSTD()));
    if(_server->getDatabase()->authLogin(login,hashPass)){
        accept();
    }
}
