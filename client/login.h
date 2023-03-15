#ifndef LOGIN_H
#define LOGIN_H

#include <QWidget>
#include <iostream>
#include "user_data.h"
#include <QTcpSocket>
namespace Ui {
class login;
}

class login : public QWidget
{
    Q_OBJECT

public:
    explicit login(QWidget *parent = nullptr);
    ~login();
    int sign_type;
    QString name;
    QString passwd;
    QTcpSocket * socket;
    bool log_ok = false;

public:
    bool valid_name();

private slots:
    //void on_pushButton_clicked();

    void on_pushButton_in_clicked();

    void on_pushButton_up_clicked();

    void slotDataArrive();

private:
    Ui::login *ui;
};

#endif // LOGIN_H
