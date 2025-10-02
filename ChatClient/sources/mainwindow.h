#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "client.h"
#include <QMainWindow>

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QString userName, Client* client,/* Net* server,*/ QWidget *parent = nullptr);
    ~MainWindow();
    static MainWindow* createClient();
signals:
    void updPrivateMsgsFromServer(const QString login);

public slots:
    void sendMsgAll();
    void sendMsgPrivate();
    void showUsers();

private:
    void initConnections();
    void updAllMessages();
    void updPrivateMessages();

private:
    Ui::MainWindow *ui;
    Client* _client;
    QString _userName;
};
#endif // MAINWINDOW_H
