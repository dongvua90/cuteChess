#ifndef LICHESSDATA_H
#define LICHESSDATA_H
#include <QString>
#include <QStringList>

class LichessData
{
public:
    LichessData();

    enum Variant{STANDARD,CRAZYHOUSE,CHESS960,KINGOFTHEHILL,THREECHECK,ANTICHESS,ATOMIC,HORDE,RACINGKINGS,FROMPOSITION};
    enum TimeType{UNLIMIT,TIMELIMIT};
    enum TypeGame{GAMEFULL,GAMESTATE};

    QString id;
//    enum Variant variant;
    QString variant;
    enum TimeType timetype;
    bool rated;

    QString white_name;
    int white_rating;
    bool white_isEngine;
    int white_timeRemain;
    bool white_draw;

    QString black_name;
    int black_rating;
    bool black_isEngine;
    int black_timeReamin;
    bool black_draw;

    QString initialFen;
    enum TypeGame typegame;
    QString status;
    QString moves;
};

class LichessMoves
{
public:
    enum GameState{STATE_RUNNING=0,STATE_WHITE_WIN,STATE_BLACK_WIN,STATE_DRAW} gamestate;
    QStringList moves;
    QString move;
    int moves_length;
    int wtime;
    int btime;
    bool wdraw;
    bool bdraw;
    QString status;
};

#endif // LICHESSDATA_H
