#include "registrationform.h"
#include "ui_registrationform.h"

RegistrationForm::RegistrationForm(QWidget *parent)
    :QWidget(parent),
     ui(new Ui::RegistrationForm)
{
    ui->setupUi(this);
    connect(ui->loginButton, &QPushButton::clicked,
            this, &RegistrationForm::toLoginForm);
    connect(ui->registerButton, &QDialogButtonBox::accepted,
            this, &RegistrationForm::pressRegOK);
    connect(ui->registerButton, &QDialogButtonBox::rejected,
            this, &RegistrationForm::pressRegCancel);
}

RegistrationForm::~RegistrationForm(){
  delete ui;
}

void RegistrationForm::toLoginForm(){
  emit loginRequested();
}

void RegistrationForm::pressRegCancel(){
    emit rejected();
}

void RegistrationForm::pressRegOK(){

    QString login = ui->loginEdit->text().trimmed();
    QString pass = ui->passwordEdit->text().trimmed();
    QString confirmPass = ui->passwordConfirmEdit->text().trimmed();

    if (login.isEmpty() ) {
        QMessageBox::warning(this, tr("Error"), tr("Login is empty"));
        return;
    }
    if (login.length() < 4) {
        QMessageBox::warning(this, tr("Error"), tr("Login must be at least 4 characters long"));
        return;
    }
    if (pass != confirmPass) {
        QMessageBox::critical(this, tr("Error"), tr("Passwords do not match"));
        return;
    }
    emit regUserSignal(login, pass);
}
