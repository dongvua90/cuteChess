#ifndef ROBOT_H
#define ROBOT_H
#include <QObject>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <err.h>
#include <errno.h>

#include <linux/types.h>
#include <linux/i2c-dev.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <linux/input.h>
#include <QTimer>
#include <QObject>
#include "threadir.h"
#include "board/genericmove.h"

class Robot :public QObject
{
    Q_OBJECT
public:
    enum MOVETYPE{MOVE_PIECE=1,MOVE_KILL,MOVE_CASLLINGG_QUEEN,MOVE_CASLLING_KING,MOVE_HOME,MOVE_PASSANT};

    Robot();
    uint8_t data_board[64];             // chứa data được đọc từ bàn cờ
     void printBoard(uint8_t dat[64]);
public slots:
    void humanEnabled(bool isHuman);
    void robotMakeMove(const Chess::GenericMove& move);
    void makeMove(int qFrom,int qTo,enum MOVETYPE move_type);
    void setFenOrigin(QString fen);
    void requestHumanMove();
    void humanEnter();
    void checkErrorBoard();
    void onCheckMoveIsError();
    void onCheckMoveIsOk();
   signals:
    void onMoveFinish();
    void onPieceError(uint8_t board[64]);
    void onHumanMakeMove(uint8_t qFrom,uint8_t qTo,enum MOVETYPE move_type);
    void onHumanMoveError();
    void onBatteryChanged(int bat,bool isChanger);
private:
    /*Vaiable for Engine Move*/
    enum MOVE_STATE{MOVE_NULL=0,MOVE_BEGIN,MOVE_STARTED,MOVE_RUNNING}engineMoveState;
    int engine_qFrom,engine_qTo;        //chứa vị trí ô di chuyển được gửi từ EngineChess.
    MOVETYPE engine_movetype;
    int tik_checkfinish=0;
    /*Vaiable for Fen change*/
//    uint8_t data_board[64];             // chứa data được đọc từ bàn cờ
    uint8_t data_fen[64];               // chứa data chuẩn của game
    uint8_t list_square_error[64];      // chứa vị trí ô piece bị lỗi.
    uint8_t number_square_error;
    /*Vaiable for Human Make Move */
    enum HUMAN_MOVE{HUMAN_NULL=0,HUMAN_BEGIN,HUMAN_WAITING,HUMAN_MOVECHECKING,HUMAN_MOVEOK}human_turn;
    uint8_t board_human_before[64];
    uint8_t board_human_after[64];
    bool isHumanEnter = false;

    uint16_t battery,battery_save;
    bool battery_is_changer,battery_is_changer_save;
    bool move_is_running;
    bool board_is_changed;
    struct input_event ev[4];
    int fd_ir;          // fd for check IR button
    QTimer *loopTime;
    int tik_readInfo=0;
    int tik_emitBoard = 0;


    int readSensorBoard(); // sau lệnh đọc này dữ liệu được lưu vào data_board[]
    int readInfoMainBoard();
    int writeMoveMainBoard(uint8_t qfrom,uint8_t qto,enum MOVETYPE type);
    /* Action for check Button IR*/
    int initIrFile();
    int calculatorMove(uint8_t *board_before,uint8_t *board_after);
//    void printBoard(uint8_t dat[64]);

     QTimer *timeCheckError;

    void convertFenToArraybyte(QString fen);
    void timeloop();
    bool isHumanTurn = false;
};

#endif // ROBOT_H
