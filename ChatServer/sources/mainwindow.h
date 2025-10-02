#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "server.h"
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
    MainWindow(QString userName, Net* server, QWidget *parent = nullptr);
    ~MainWindow();
    static MainWindow* createClient();
signals:

public slots:
    void blockUser();
    void unblockUser();

private:
    void initConnections();
    void updAllMessages();

private:
    Ui::MainWindow *ui;
    Net* _server;
    QString _userName;
};
#endif // MAINWINDOW_H
