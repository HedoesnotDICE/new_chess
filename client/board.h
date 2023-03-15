#ifndef BOARD_H
#define BOARD_H

#include <QFrame>
#include "Stone.h"
#include "Step.h"
#include <QVector>
#include <QLabel>

//git here
class Board : public QFrame
{
    Q_OBJECT
public:
    explicit Board(QWidget *parent = 0);
    ~Board();

    /* game data */
    Stone   _s[32];
    int     _r;
    QPoint  _off;
    bool    _bSide; // beRedSide

    QVector<Step*>  _steps;

    /* game status */
    int     _selectid;
    bool    _bRedTurn;
    bool    _over;

    QLabel  *_turn;
    QLabel  *_gameover;
    void    init(bool bRedSide);
    void    setLabel();

    /* draw functions */
    void    paintEvent(QPaintEvent *);
    void    drawPlate(QPainter& p);
    void    drawPlace(QPainter& p);
    void    drawInitPosition(QPainter& p);
    void    drawInitPosition(QPainter& p, int row, int col);
    void    drawStone(QPainter& p);
    void    drawStone(QPainter& p, int id);

    /* function for coordinate */
    QPoint  center(int row, int col);
    QPoint  center(int id);
    QPoint  topLeft(int row, int col);
    QPoint  topLeft(int id);
    QRect   cell(int row, int col);
    QRect   cell(int id);

    bool    getClickRowCol(QPoint pt, int& row, int& col);

    /* help function */
    QString     name(int id);
    bool        red(int id);
    bool        sameColor(int id1, int id2);
    int         getStoneId(int row, int col);
    virtual void    killStone(int id);
    void        reliveStone(int id);
    void        moveStone(int moveid, int row, int col);
    bool        isDead(int id);
    bool        isVisible(int id);
    bool        isAttackable(int attackerId, int victimId);
    int         occupiedId(int row, int col);

    /* move stone */
    void    mouseReleaseEvent(QMouseEvent *);
    void    click(QPoint pt);
    virtual void click(int id, int row, int col);
    void    trySelectStone(int id);
    void    tryMoveStone(int killid, int row, int col);
    void    moveStone(int moveid, int killid, int row, int col);
    void    saveStep(int moveid, int killid, int row, int col, QVector<Step*>& steps);
    void    backOne();
    void    back(Step* step);
    virtual void back();

    /* rule */
    bool    canMove(int moveid, int killid, int row, int col);
    bool    canMoveChe(int moveid, int killid, int row, int col);
    bool    canMoveMa(int moveid, int killid, int row, int col);
    bool    canMovePao(int moveid, int killid, int row, int col);
    bool    canMoveBing(int moveid, int killid, int row, int col);
    bool    canMoveJiang(int moveid, int killid, int row, int col);
    bool    canMoveShi(int moveid, int killid, int row, int col);
    bool    canMoveXiang(int moveid, int killid, int row, int col);

    bool    canSelect(int id);

    /* rule helper */
    int     getStoneCountAtLine(int row1, int col1, int row2, int col2);
    int     relation(int row1, int col1, int row, int col);
    bool    isBottomSide(int id);

signals:
    void sgGameOver(bool redwin);

public slots:
    void    slotBack();
    virtual void    slotGameOver(bool redwin);

};

#endif // BOARD_H
