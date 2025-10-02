#ifndef STARTSCREEN_H
#define STARTSCREEN_H

#include <QDialog>
#include "server.h"
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
    Net* getServer() const;
    QString userName() const;

public slots:
    void setLoginForm();
    void setRegistrationForm();
    void onRejectRequested();
    void RegUser(const QString& login, const QString& pwd);
    void AuthUser(const QString& login, const QString& pwd);

// signals:
//     //void reqAuth();

private:
    void initConnections();

    Ui::StartScreen* ui;
    QString _userName;
    Net* _server;
    SHA256 _sha256;

};

#endif
