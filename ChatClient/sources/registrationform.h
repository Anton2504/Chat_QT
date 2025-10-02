#ifndef REGISTRATIONFORM_H
#define REGISTRATIONFORM_H

#include <QWidget>
#include <QMessageBox>
namespace Ui {
class RegistrationForm;
}

class RegistrationForm : public QWidget{

    Q_OBJECT

public:
    explicit RegistrationForm(QWidget *parent = nullptr);
    ~RegistrationForm();

signals:
    void loginRequested();
    void regUserSignal(const QString& login, const QString& pwd);
    void rejected();

private slots:
    void toLoginForm();
    void pressRegOK();
    void pressRegCancel();

private:
    Ui::RegistrationForm *ui;
};

#endif
