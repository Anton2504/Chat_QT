#ifndef LOGINFORM_H
#define LOGINFORM_H

#include <QWidget>
#include <QMessageBox>

namespace Ui {
class LoginForm;
}

class LoginForm : public QWidget
{
    Q_OBJECT

public:
    explicit LoginForm(QWidget *parent = nullptr);
    ~LoginForm();

signals:
    void registerRequested();
    void accepted();
    void rejected();
    void authUserSignal(const QString& login, const QString& pwd);

private slots:
    void pressOK();
    void pressCancel();
    void toRegistrationForm();

private:
  Ui::LoginForm *ui;  
};

#endif
