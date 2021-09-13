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
#include <QProcess>

class Robot :public QObject
{
    Q_OBJECT
public:
    enum MOVETYPE{MOVE_PIECE=1,MOVE_KILL,MOVE_CASLLINGG_QUEEN,MOVE_CASLLING_KING,MOVE_PASSANT,MOVE_HOME};
    Robot();
    uint8_t data_board[64];                 // chứa data được đọc từ bàn cờ
    uint8_t compression_board[32];
    void printBoard(uint8_t dat[64]);

public slots:
     void requestMakeMove();                // yêu cầu người chơi tạo 1 nước đi
     void boardMoveError();                 // người chơi đi nước đi bị lỗi
     void onButtonEnter();                  // nút nhấn IR xác nhận nước đi
     void onButtonTest();                   // nút nhấn test
     void armRobotMove(QString mov);
   signals:
    void armRobotMovedFinish();             // phát ra lệnh sau khi robot di chuyển xong 1 nước đi
    void boardSensorMakeMove(QString mov);  // người chơi tạo 1 nước đi
    void onHumanMoveError();                // phát ra lệnh khi người chơi di chuyển bị lỗi sơ cấp
    void onPieceError(uint8_t board[64]);   // gửi thông tin bàn cờ đến boardScene, những quân cờ không có thực trên bàn cờ sẽ bị làm mờ
private:
    /*Vaiable for Engine Move*/
    int engine_qFrom,engine_qTo;        //chứa vị trí ô di chuyển được gửi từ EngineChess.
    MOVETYPE engine_movetype;
    int tik_checkfinish=0;

    /*Vaiable for Fen change*/
    uint8_t data_fen[64];               // chứa data chuẩn của game

    QTimer *loopTime;

    int readSensorBoard();              // sau lệnh đọc này dữ liệu được lưu vào data_board[]
    int i2c_readInfoMainBoard();            // đọc các thông tin về battery,charger,moveFinish
    int i2c_writeMoveMainBoard(uint8_t qfrom,uint8_t qto,enum MOVETYPE type);   //gửi lệnh di chuyển đến robot

    int tik_taskStatus=0;
    void taskStatus(); //frequency update 1Hz
    int step_arm_move = 0,tik_delay_move_finish =0,tik_delay_edit_piece=0;
    void taskArmMove(); // xử lý task robot di chuyển 1 nước đi
    int step_getHumanMove=0,tik_timleft_edit_piece=0;
    uint8_t board_human_before[64],board_human_after[64];
    bool isHumanEnter = false;
    void taskGetHumanMove();


    uint8_t extractBoard(uint8_t dat);
    void extractBoardAll();

    void makeMove(int qFrom, int qTo, Robot::MOVETYPE move_type);

    /* Action for check Button IR*/
    int initIrFile();
    int calculatorMove(uint8_t *board_before,uint8_t *board_after);
    void timeloop();

    void updateWifiSignal();


    QString convertSquareToMove(int qFrom,int qTo);

};

#endif // ROBOT_H
