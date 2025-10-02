#include "loginform.h"
#include "ui_loginform.h"

LoginForm::LoginForm(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::LoginForm)
{
    ui->setupUi(this);
    connect(ui->registrationButton, &QPushButton::clicked,
            this, &LoginForm::toRegistrationForm);
    connect(ui->loginButton, &QDialogButtonBox::accepted,
            this, &LoginForm::pressOK);
    connect(ui->loginButton, &QDialogButtonBox::rejected,
            this, &LoginForm::pressCancel);
}

LoginForm::~LoginForm()
{
    delete ui;
}

void LoginForm::pressOK()
{
    QString login = ui->loginEdit->text().trimmed();
    QString pass = ui->passwordEdit->text().trimmed();

    if (login.isEmpty() ) {
        QMessageBox::warning(this, tr("Error"), tr("Login is empty"));
        return;
    }
    if (login.length() < 4) {
        QMessageBox::warning(this, tr("Error"), tr("Login must be at least 4 characters long"));
        return;
    }
    if (pass.isEmpty()) {
        QMessageBox::critical(this, tr("Error"), tr("Passwords is empty"));
        return;
    }
    emit authUserSignal(login,pass);
}


void LoginForm::pressCancel()
{
    emit rejected();
}


void LoginForm::toRegistrationForm()
{
    emit registerRequested();
}









































// LoginForm::LoginForm(QWidget *parent) :
//   QWidget(parent),
//   ui(new Ui::LoginForm)
// {
//   ui->setupUi(this);
// }

// LoginForm::~LoginForm()
// {
//   delete ui;
// }

// void LoginForm::setClient(Client* c)
// {
//     _client = c;
//     connect(_client, &Client::loginResult, this, &LoginForm::onLoginResult);
// }


// void LoginForm::setDatabase(std::shared_ptr<Database> dbPtr)
// {
//   m_dbPtr = dbPtr;
// }

// void LoginForm::pressOK()
// {
//     if (!_client) return;

//     QString username = ui->loginEdit->text().trimmed();
//     QString password = ui->passwordEdit->text().trimmed();

//     if (username.isEmpty() || password.isEmpty()) {
//         QMessageBox::warning(this, tr("Error"), tr("Username and password cannot be empty"));
//         return;
//     }

//     _client->login(username, password);


//   // auto userId = m_dbPtr->checkPassword(ui->loginEdit->text().toStdString(),
//   //                                      ui->passwordEdit->text().toStdString());
//   // if(userId == -1)
//   // {
//   //   QMessageBox::critical(this,
//   //                         tr("Error"),
//   //                         tr("Password is wrong"));
//   //   return;
//   // }

//   // emit accepted(userId, ui->loginEdit->text());
// }

// void LoginForm::pressCancel()
// {
//   emit rejected();
// }

// void LoginForm::toRegistrationForm()
// {
//   emit registerRequested();
// }

// void LoginForm::onLoginResult(bool success, /*int userId, */const QString& userName/*, bool isAdmin*/)
// {
//     qDebug() << "LoginForm::onLoginResult success:" << success;
//     if (success) {
//         emit accepted(/*userId, */userName/*, isAdmin*/);
//     } else {
//         QMessageBox::critical(this, tr("Login Failed"), tr("Invalid credentials or user is banned."));
//     }
// }




