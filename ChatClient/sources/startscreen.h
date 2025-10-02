#ifndef STARTSCREEN_H
#define STARTSCREEN_H

#include <QDialog>
#include "client.h"
#include "sha256.h"
#include <QJsonObject>
#include <QMessageBox>

namespace Ui {
class StartScreen;
}

class StartScreen : public QDialog{

    Q_OBJECT

public:
    explicit StartScreen(QWidget *parent = nullptr);
    ~StartScreen();
    Client* getSocket() const;
    QString userName() const;

public slots:
    void setLoginForm();
    void setRegistrationForm();
    void onLoggedIn(QJsonObject& pkg);
    void onRejectRequested();
    void RegUser(const QString& login, const QString& pwd);
    void RegUserResponce(QJsonObject regPkg);
    void AuthUser(const QString& login, const QString& pwd);
    void AuthUserResponce(QJsonObject authPkg);

signals:
    void reqAuth();

private:
    void initConnections();

    Ui::StartScreen* ui;
    QString _userName;
    Client* _client;
    SHA256 _sha256;

};

#endif
