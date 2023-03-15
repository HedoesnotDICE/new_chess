#include "netgame.h"

NetGame::NetGame(bool server, QWidget *parent) : Board(parent)
{
    _inGame = false;
    _server = NULL;
    _socket = NULL;
    _peer_socket = NULL;
    _bServer = server;
    msg::Login_info game_info;
    std::string sock_data;

    if(_bServer)
    {
        _server = new QTcpServer(this);
        _server->listen(QHostAddress::Any, 9899);
        connect(_server, SIGNAL(newConnection()), this, SLOT(slotNewConnection()));
    }
    else
    {
        _socket = new QTcpSocket(this);
        _socket->connectToHost(ServIP, ServPort);
        connect(_socket, SIGNAL(readyRead()), this, SLOT(slotDataArrive()));
        game_info.set_sign_type(EN_NEW_GAME);
        game_info.set_name(UserData::GetInstance()->name.toStdString());
        game_info.SerializeToString(&sock_data);
        _socket->write(sock_data.c_str());
    }
}

void NetGame::slotNewConnection()
{
    if(!_inGame) return;
    std::cout << "before connect" << std::endl;
    _peer_socket = _server->nextPendingConnection();

    connect(_peer_socket, SIGNAL(readyRead()), this, SLOT(slotInGameDataArrive()));
    std::cout << "after connect" << std::endl;

    /* 产生随机数来决定谁走红色 */
    bool bRedSide = qrand()%2>0;

    init(bRedSide);

    /* 发送给对方 */
    msg::InGameInfo ingame_info;
    ingame_info.set_in_game_type(EN_IN_GAME_INFO_INITIAL);
    ingame_info.set_beredside(!bRedSide);
    std::string sock_data;
    ingame_info.SerializeToString(&sock_data);
    _peer_socket->write(sock_data.c_str());
}

void NetGame::back()
{
    if(_bRedTurn != _bSide)
        return;
    backOne();
    backOne();

    QByteArray buf;
    buf.append(3);
    _socket->write(buf);
}

void NetGame::click(int id, int row, int col)
{
    if(_bRedTurn != _bSide)
        return;

    /* 发送给对方 */
    if(!_socket) return;
    msg::InGameInfo ingame_msg;
    ingame_msg.set_in_game_type(EN_IN_GAME_INFO_NORMAL);
    ingame_msg.set_id(id);
    ingame_msg.set_row(row);
    ingame_msg.set_col(col);
    std::string sock_data;
    ingame_msg.SerializeToString(&sock_data);
    _peer_socket->write(sock_data.c_str());


    Board::click(id, row, col);

    qDebug() << "send data success";
}

void NetGame::backFromNetwork(QByteArray)
{
    backOne();
    backOne();
}
void NetGame::clickFromNetwork(int id, int row, int col)
{
    Board::click(id, 9-row, 8-col);
}
void NetGame::initFromNetwork(QByteArray buf)
{
    bool bRedSide = buf.at(1)>0?true:false;
    init(bRedSide);
}

void NetGame::beServer(QByteArray buf)
{
    if(_socket)
    {
        _socket->close();
        delete _socket;
        _socket = nullptr;
    }
    _server = new QTcpServer(this);
    std::cout << "step1" << std::endl;
    _server->listen(QHostAddress::Any, 9899);
    std::cout << "step2" << std::endl;
    connect(_server, SIGNAL(newConnection()), this, SLOT(slotNewConnection()));
    std::cout << "step3" << std::endl;
}

void NetGame::beClient(std::string passiveIP)
{
    if(_socket)
    {
        _socket->close();
        delete _socket;
        _socket = nullptr;
    }
    std::cout << "try to connect to " << passiveIP << std::endl;
    _socket = new QTcpSocket(this);
    QString ip = QString::fromStdString(passiveIP);
    _socket->connectToHost(ip, 9899);
    connect(_socket, SIGNAL(readyRead()), this, SLOT(slotDataArrive()));
}

void NetGame::slotDataArrive()
{
    QByteArray buf = _socket->readAll();
    if(_inGame)
    {

    }
    else
    {
        msg::Login_info net_msg;
        net_msg.ParseFromArray(buf.data(), buf.size());
        uint32_t sign_type = net_msg.sign_type();
        if(sign_type == EN_NEW_GAME)
        {
            uint32_t result = net_msg.result();
            uint32_t game_seq = net_msg.game_seq();
            UserData::GetInstance()->game_seq = game_seq;
            if(EN_NEW_GAME_RSP_CLIENT == result)
            {

                _inGame = true;
                _bServer = false;

                QString ip = QString::fromStdString(net_msg.ip());
                if(_peer_socket == NULL)
                {
                    _peer_socket = new QTcpSocket(this);
                }
                _peer_socket->connectToHost(ip, ClientPort);
                connect(_peer_socket, SIGNAL(readyRead()), this, SLOT(slotInGameDataArrive()));
            }
            else if(EN_NEW_GAME_RSP_SERVER == result)
            {

                _inGame = true;
                _bServer = true;

                if(_server == NULL) _server = new QTcpServer(this);
                _server->listen(QHostAddress::Any, ClientPort);
                connect(_server, SIGNAL(newConnection()), this, SLOT(slotNewConnection()));
            }
            else
            {
                qDebug() << "unknown new_game rsponse";
            }
        }
        else if(sign_type == EN_END_GAME)
        {

        }
        else
        {
            qDebug() << "unknown sign type:" << sign_type;
        }
    }
}

void NetGame::slotInGameDataArrive()
{
    if(!_inGame) return ;

    QByteArray buf = _peer_socket->readAll();
    msg::InGameInfo game_info;
    game_info.ParseFromArray(buf.data(), buf.size());

    uint32_t type = game_info.in_game_type();
    if(EN_IN_GAME_INFO_INITIAL == type)
    {
        bool beRedSide = game_info.beredside();
        qDebug() << "board initialize from network, " << (beRedSide ? "redSizd" : "blackSize");
        init(beRedSide);
    }
    else if(EN_IN_GAME_INFO_NORMAL == type)
    {
        int id, row, col;
        id = game_info.id();
        row = game_info.row();
        col = game_info.col();
        qDebug() << "move id:" << id;
        clickFromNetwork(id, row, col);
    }
    else
    {
        qDebug() << "unknown in_game type";
    }
}

void NetGame::slotGameOver(bool redwin)
{
    qDebug() << "over" ;
    _gameover->setText("红胜");
    if(!redwin) _gameover->setText("黑胜");
    _gameover->show();

    //只需客户端玩家发起结算即可
    if(_bServer) return ;

    msg::Login_info game_info;
    game_info.set_sign_type(EN_END_GAME);
    game_info.set_name(UserData::GetInstance()->name.toStdString());
    game_info.set_game_seq(UserData::GetInstance()->game_seq);
    if(_bSide)
    {
        game_info.set_won(redwin);
    }
    else
    {
        game_info.set_won(!redwin);
    }
    std::string data;
    game_info.SerializeToString(&data);

    _socket->connectToHost(ServIP, ServPort);
    _socket->write(data.c_str());

}

NetGame::~NetGame()
{

}
