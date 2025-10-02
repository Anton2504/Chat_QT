#include "startscreen.h"
#include "ui_startscreen.h"



StartScreen::StartScreen(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::StartScreen)   {

    ui->setupUi(this);
    _client = new Client;
    initConnections();
}

StartScreen::~StartScreen() {
    delete ui;
}

Client *StartScreen::getSocket() const  {
    return _client;
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
    connect(_client, &Client::regResponceJson, this, &StartScreen::RegUserResponce);

    connect(ui->loginWidget, &LoginForm::authUserSignal, this, &StartScreen::AuthUser);
    connect(_client, &Client::authResponceJson, this, &StartScreen::AuthUserResponce);
}

void StartScreen::onRejectRequested(){
    reject();
}

void StartScreen::RegUser(const QString &login, const QString &pwd){
    _sha256.update(pwd);
    QString hashPass = QString::fromStdString(_sha256.backToString
                                              (_sha256.digestSTD()));
    QJsonObject regUser;
    regUser.insert("cmd", "reg_user");
    regUser.insert("login", login);
    regUser.insert("pass", hashPass);
    _client->sendToServer(regUser);

}

void StartScreen::RegUserResponce(QJsonObject regPkg)   {

    if(regPkg["status"].toBool()){
        onLoggedIn(regPkg);
    } else{
        QMessageBox::critical(this, "Error", regPkg["info"].toString());
    }
}

void StartScreen::AuthUser(const QString &login, const QString &pwd)    {
    _sha256.update(pwd);
    QString hashPass = QString::fromStdString(_sha256.backToString
                                              (_sha256.digestSTD()));
    QJsonObject authUser;
    authUser.insert("cmd", "auth_user");
    authUser.insert("login", login);
    authUser.insert("pass", hashPass);
    _client->sendToServer(authUser);
}

void StartScreen::AuthUserResponce(QJsonObject authPkg) {

    if(authPkg["status"].toBool()){
        onLoggedIn(authPkg);
    } else{
        QMessageBox::critical(this, "Error", authPkg["info"].toString());
    }
}

void StartScreen::onLoggedIn(QJsonObject &pkg){

    _userName = pkg["login"].toString();
    accept();
}


































// void StartScreen::onLoggedIn(/*uint userId, */QString userName/*, bool isAdmin*/)
// {
//     qDebug() << "StartScreen::onLoggedIn called with user:" << userName;
//     //m_userId = userId;
//     _userName = userName;

//     // if (isAdmin) {
//     //     emit adminLoggedIn();
//     //     hide();
//     //     return;
//     // }

//     emit accepted(/*userId, */userName/*, isAdmin*/);
//     accept();
//     return;
// }


















/*
StartScreen::StartScreen(std::shared_ptr<Database> dbPtr,
                         Client* clientPtr,
                         QWidget *parent) :
  QDialog(parent),
    ui(new Ui::StartScreen),
    _client(clientPtr)
{
  ui->setupUi(this);

  if(dbPtr)
    m_dbPtr = dbPtr;
  else
    m_dbPtr = make_shared<Database>();

  ui->loginWidget->setDatabase(m_dbPtr);
  ui->registerWidget->setDatabase(m_dbPtr);

  ui->loginWidget->setClient(_client);
  ui->registerWidget->setClient(_client);

  connect(ui->loginWidget, &LoginForm::registerRequested, this, &StartScreen::setRegistrationForm);
  connect(ui->loginWidget, &LoginForm::accepted, this, &StartScreen::onLoggedIn);
  connect(ui->loginWidget, &LoginForm::rejected, this, &StartScreen::onRejectRequested);
  connect(ui->registerWidget, &RegistrationForm::loginRequested, this, &StartScreen::setLoginForm);
  connect(ui->registerWidget, &RegistrationForm::accepted, this, &StartScreen::onLoggedIn);
  connect(ui->registerWidget, &RegistrationForm::rejected, this, &StartScreen::onRejectRequested);
}

//#include <memory>
//#include "Databasex.h"



StartScreen::~StartScreen()
{
  delete ui;
}

void StartScreen::setLoginForm()
{
  ui->stackedWidget->setCurrentIndex(0);
}

void StartScreen::setRegistrationForm()
{
  ui->stackedWidget->setCurrentIndex(1);
}

// void StartScreen::onLoggedIn(uint userId, QString userName)
// {
//   m_userId = userId;
//   m_userName = userName;
//   accept();
// }
*/

// void StartScreen::onLoggedIn(/*uint userId, */QString userName/*, bool isAdmin*/)
// {
//     qDebug() << "StartScreen::onLoggedIn called with user:" << userName;
//     //m_userId = userId;
//     _userName = userName;

//     // if (isAdmin) {
//     //     emit adminLoggedIn();
//     //     hide();
//     //     return;
//     // }

//     emit accepted(/*userId, */userName/*, isAdmin*/);
//     accept();
//     return;
// }

// void StartScreen::onRejectRequested()
// {
//   reject();
// }

// std::shared_ptr<Database> StartScreen::getDatabase() const
// {
//   return m_dbPtr;
// }

// QString StartScreen::userName() const
// {
//   return _userName;
// }

// // int StartScreen::userId() const
// // {
// //   return m_userId;
// // }








