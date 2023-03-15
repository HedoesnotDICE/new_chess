#include <QApplication>
#include "choosedlg.h"
#include "netgame.h"
#include "mainwnd.h"
#include "login.h"
#include <QTime>
#include <QTcpServer>
#include <QTcpSocket>
#include "myhead.h"

extern const QString ServIP = "192.168.1.9";
extern const int ServPort = 9899;
extern const int ClientPort = 9999;
QTcpServer * server;
QTcpSocket * socket;

ChooseDlg *dlg  =   nullptr;
MainWnd * wnd   =   nullptr;
void newGame()
{
    if(dlg)
    {
        delete dlg;
    }
    if(wnd)
    {
        wnd->hide();
        delete wnd;
    }

    dlg = new ChooseDlg;
    if(dlg->exec() != QDialog::Accepted)
    {
        if(dlg) delete dlg;
        dlg = nullptr;
        return ;
    }
    wnd = new MainWnd(dlg->_selected);
    wnd->show();
}

int main(int argc, char* argv[])
{
    QApplication app(argc, argv);

    QTime t = QTime::currentTime();
    qsrand(t.msec()+t.second()*1000);

    server = new QTcpServer;
    socket = new QTcpSocket;
    socket->connectToHost(ServIP, ServPort);

//    ChooseDlg dlg;
//    MainWnd wnd;
//    if(dlg.exec() != QDialog::Accepted)
//        return 0;
//    MainWnd wnd(dlg._selected);
//    wnd.show();

    login login_wnd;
    login_wnd.show();

    return app.exec();
}
