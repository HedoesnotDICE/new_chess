#include "login.h"
#include "ui_login.h"
#include "choosedlg.h"
#include "mainwnd.h"
#include <QTcpSocket>
#include <iostream>
#include <QMessageBox>
#include "net_msg.pb.h"
#include "my_types.h"
#include "tool.h"
extern void newGame();
extern const QString ServIP;
extern const int ServPort;
login::login(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::login)
{
    ui->setupUi(this);
    socket = NULL;
   // connect(socket, SIGNAL(readyRead()), this, SLOT(slotDataArrive()));
}

login::~login()
{
    delete ui;
}

bool login::valid_name()
{
    if(name.size() > 20 || passwd.size() > 20) return false;
    return true;
}

void login::on_pushButton_in_clicked()
{
    if(socket)
    {
        socket->close();
        delete socket;
    }
    socket = new QTcpSocket();
    socket->connectToHost(ServIP, ServPort);
    connect(socket, SIGNAL(readyRead()), this, SLOT(slotDataArrive()));
    name = ui->lineEdit_name->text();
    passwd = ui->lineEdit_passwd->text();
    if(!valid_name())
    {
        QMessageBox::information(this, "register info", "用户名或密码不合格");
    }
    else
    {
        msg::Login_info lg_info;
        sign_type = EN_SIGN_IN;
        lg_info.set_sign_type(sign_type);
        lg_info.set_name(name.toStdString());
        lg_info.set_passwd(passwd.toStdString());
        std::string info;
        lg_info.SerializeToString(&info);
        socket->write(info.c_str());
    }
}


void login::on_pushButton_up_clicked()
{
    if(socket)
    {
        socket->close();
        delete socket;
    }
    socket = new QTcpSocket();
    socket->connectToHost(ServIP, ServPort);
    connect(socket, SIGNAL(readyRead()), this, SLOT(slotDataArrive()));
    name = ui->lineEdit_name->text();
    passwd = ui->lineEdit_passwd->text();
    if(!valid_name())
    {
        QMessageBox::information(this, "register info", "用户名或密码不能包含空格");
    }
    else
    {
        msg::Login_info lg_info;
        sign_type = EN_SIGN_UP;
        lg_info.set_sign_type(sign_type);
        lg_info.set_name(name.toStdString());
        lg_info.set_passwd(passwd.toStdString());
        std::string info;
        lg_info.SerializeToString(&info);
        socket->write(info.c_str());
    }
}

void login::slotDataArrive()
{
    QByteArray response = socket->readAll();
    msg::Login_info info;
    info.ParseFromString(response.data());
    int sign_type = info.sign_type();
    uint32_t result =info.result();
    if(sign_type ==EN_SIGN_IN)
    {
        if(result == EN_LOGIN_SCCUSS)
        {
            UserData::GetInstance()->name = ui->lineEdit_name->text();
            this->hide();
//            ChooseDlg dlg;
//            if(dlg.exec() != QDialog::Accepted)
//                return ;

//            MainWnd * wnd = new MainWnd(dlg._selected);
//            wnd->show();
            newGame();
        }
        else
        {
            //no username or passwd wrong
            QMessageBox::information(this, "login info", en2string(result));
        }
    }
    else
    {
        if(result == EN_LOGIN_SCCUSS)
        {
            QMessageBox::information(this, "register info", en2string(result));
        }
        else
        {
            //username exist
            QMessageBox::information(this, "register info", en2string(result));
        }
    }
}

