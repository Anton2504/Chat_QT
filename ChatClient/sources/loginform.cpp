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
