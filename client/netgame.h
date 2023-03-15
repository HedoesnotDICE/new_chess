#ifndef NETGAME_H
#define NETGAME_H

#include "net_msg.pb.h"
#include "in_game_msg.pb.h"
#include "board.h"
#include "my_types.h"
#include <QTcpServer>
#include <QTcpSocket>
#include <QString>
#include "user_data.h"
#include <synchapi.h>

/*
 * 报文格式定义
 * 1.初始化    两个字节  （第一个字节固定一，第二个字节0,1 1表示接收方走红旗 0表示走黑棋
 * 2.鼠标点击   四个字节
 * 3.悔棋      一个字节
 *
*/


extern const QString ServIP;
extern const int ServPort;
extern const int ClientPort;

class NetGame : public Board
{
    Q_OBJECT
public:
    explicit NetGame(bool server, QWidget *parent = 0);
    ~NetGame();

    bool _bServer;
    QTcpServer* _server;
    QTcpSocket* _socket;
    QTcpSocket* _peer_socket;
    bool _inGame;

    void back();
    void click(int id, int row, int col);

    void backFromNetwork(QByteArray buf);
    void clickFromNetwork(int id, int row, int col);
    void initFromNetwork(QByteArray buf);
    void beServer(QByteArray buf);
    void beClient(std::string passiveIP);
signals:

public slots:
    void slotNewConnection();
    void slotDataArrive();
    void slotInGameDataArrive();
    void slotGameOver(bool redwin);
};

#endif // NETGAME_H
