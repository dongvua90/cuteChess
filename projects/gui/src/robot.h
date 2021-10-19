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

#define MOVE_PIECE				1
#define MOVE_HOME				2
#define MOVE_KILL				3
#define MOVE_CASLLINGG_QUEEN  	4
#define MOVE_CASLLING_KING    	5
#define MOVE_PASSANT			6


#define CMD_INFO		'I'
#define CMD_GRIPPER 	'G'
#define CMD_PICK		'p'
#define CMD_PICKPIECE	'P'
#define CMD_MOVE		'm'
#define CMD_MOVEPIECE	'M'
#define CMD_MOVEHOME  	'H'
#define CMD_MOVETEST	'T'
#define CMD_RESETPOSDIE	'R'

class Robot :public QObject
{
    Q_OBJECT
public:
//    enum MOVETYPE{MOVE_PIECE=1,MOVE_KILL,MOVE_CASLLINGG_QUEEN,MOVE_CASLLING_KING,MOVE_PASSANT,MOVE_HOME};
    Robot();
    uint8_t data_board[64];                 // chứa data được đọc từ bàn cờ
    uint8_t compression_board[32];
public slots:
     void requestMakeMove();                // yêu cầu người chơi tạo 1 nước đi
     void boardMoveError();                 // người chơi đi nước đi bị lỗi
     void onButtonEnter();                  // nút nhấn IR xác nhận nước đi
     void onButtonTest();                   // nút nhấn test
     void armRobotMove(QString mov);
     void checkingBoardPiece();             // phát thông báo nếu Pieces trên bàn cờ sai vị trí
     int mainboardResetSquareKill();    // khởi tạo lại square kill
     void debugPrintboard();
   signals:
    void armRobotMovedFinish();             // phát ra lệnh sau khi robot di chuyển xong 1 nước đi
    void boardSensorMakeMove(QString mov);  // người chơi tạo 1 nước đi
    void onPieceError(uint8_t board[64]);   // gửi thông tin bàn cờ đến boardScene, những quân cờ không có thực trên bàn cờ sẽ bị làm mờ
    void onCheckedBoardPieces();            // phát ra lệnh sau khi checking pieces in board
private:
    /*Vaiable for Engine Move*/
    int engine_qFrom,engine_qTo;        //chứa vị trí ô di chuyển được gửi từ EngineChess.
    uint8_t engine_movetype;
    int tik_checkfinish=0;

    /*Vaiable for Fen change*/
    uint8_t data_fen[64];               // chứa data chuẩn của game

    QTimer *loopTime;

    int readSensorBoard();              // sau lệnh đọc này dữ liệu được lưu vào data_board[]
    int mainboardWritemove(uint8_t qfrom,uint8_t qto,uint8_t option);      // gửi lệnh di chuyển đến robot qua UART


    int tik_taskStatus=0;
    void taskStatus(); //frequency update 1Hz
    int step_getHumanMove=0,tik_timleft_edit_piece=0;
    uint8_t board_human_before[64],board_human_after[64];
    bool isHumanEnter = false;
    void taskGetHumanMove();
    bool is_checking_piece_position = false;
    uint8_t tik_task_checking_pieces=0;
    void taskCheckingPiecesPosition();


    uint8_t extractBoard(uint8_t dat);
    void extractBoardAll();

    QString calculatorMove(uint8_t *board_before,uint8_t *board_after);
    void timeloop();

    void updateWifiSignal();

    QString convertSquareToMove(int qFrom,int qTo);
    void printBoard(uint8_t dat[64]);
    bool arm_moved=true;

signals:
    void armMoved();


};

#endif // ROBOT_H
