#include "robot.h"
#include <QDebug>
#include "robochessapp.h"
#include "mainwindow.h"
#include "gameviewer.h"
#include "boardview/boardscene.h"

#define ADDR_BOARD 0x19
#define ADDR_MAINBOARD 0x10
#define IRCODE_ENTER 32798

Robot::Robot()
{
    loopTime = new QTimer(this);
    connect(loopTime,&QTimer::timeout,this,&Robot::timeloop);
    loopTime->start(200);
}

int Robot::readSensorBoard()
{
    int fd = open("/dev/i2c-1",O_RDWR | O_NONBLOCK);
    if(fd < 0){
        printf("I2C-Faile open i2c1\r\n");
        return -1;
    }

    if(ioctl(fd,I2C_SLAVE,ADDR_BOARD) < 0)
    {
        printf("I2C-Faile to set Address BoardSensor");
        return -1;
    }
    if(read(fd,compression_board,32)!= 32){
        printf("I2C-Board Read != 32 byte\r\n");
        return -1;
    }
    close(fd);
    extractBoardAll();
    return 0;
}

uint8_t Robot::extractBoard(uint8_t dat)
{
    uint8_t _comp;
    switch(dat){
    case 0:_comp ='_'; break;
    case 1:_comp ='R'; break;
    case 2:_comp ='N'; break;
    case 3:_comp ='B'; break;
    case 4:_comp ='Q'; break;
    case 5:_comp ='K'; break;
    case 6:_comp ='P'; break;
    case 7:_comp ='r'; break;
    case 8:_comp ='n'; break;
    case 9:_comp ='b'; break;
    case 10:_comp ='q'; break;
    case 11:_comp ='k'; break;
    case 12:_comp ='p'; break;
    }
    return _comp;
}

void Robot::extractBoardAll()
{
    for(int i=0;i<64;i++){
        if(i%2){
             data_board[i] =extractBoard(compression_board[i/2] & 15); //   15=0b00001111
         }else{
             data_board[i] =extractBoard((compression_board[i/2] & 240) >>4); // 240=0b11110000
         }
     }
}

int Robot::i2c_readInfoMainBoard()
{
    uint8_t data_rev[7];
    int fd =open("/dev/i2c-1",O_RDWR | O_NONBLOCK);
    if(fd < 0){
        printf("I2C-Faile open i2c1\r\n");
        return -1;
    }
    if(ioctl(fd,I2C_SLAVE,ADDR_MAINBOARD) < 0){
        printf("I2C-Faile to set Address Mainboard");
        return -1;
    }
    if(read(fd,data_rev,7)!= 7){
        printf("I2C-Faile write to Mainboard\r\n");
        return -1;
    }
     close(fd);
     RobochessApplication::instance()->info_battery = (data_rev[5] << 8) +data_rev[4];
     RobochessApplication::instance()->info_battery_charger = !(data_rev[6] & 0b00000100);
     RobochessApplication::instance()->info_arm_move_finish = data_rev[6] & 0b00001000;
     return 0;
}
/* Protocol i2c-interface Write: 4 Byte
* [CMD]--[DATA0]-[DATA1]-[DATA2]
* CMD: 1 => Move Piece , qFrom = DATA0 , sTo = DATA1 , option = DATA2.
* CMD: 2 => Move Home
* Protocol i2c-interface Read:	10 Byte
* [AS5600_J1]-[AS5600-J2]-[BATTERY]-[STATUS]
* 	2-Byte		2-Byte		2-Byte	  1-Byte
* 	STATUS: 0b76543210 => 0:Hal_sensor_up 1:Hal_sensor_down 2:Bat_Changer 3:isFinish
* define CMD_I2C_MOVE_PIECE				1
* define CMD_I2C_MOVE_KILL				2
* define CMD_I2C_MOVE_CASLLINGG_QUEEN  	3
* define CMD_I2C_MOVE_CASLLING_KING    	4
* define CMD_I2C_MOVE_PASSANT			5
* define CMD_I2C_MOVE_HOME				2
*/
int Robot::i2c_writeMoveMainBoard(uint8_t qfrom, uint8_t qto, Robot::MOVETYPE type)
{
    uint8_t data_send[4];
    switch (type) {
    case MOVE_HOME:
        data_send[0] =2;
        data_send[1] =0;
        data_send[2] =0;
        data_send[3] =0;
        break;
    case MOVE_CASLLING_KING:
        data_send[0] =1;
        data_send[1] =0;
        data_send[2] =0;
        data_send[3] =MOVE_CASLLING_KING;
        break;
    case MOVE_CASLLINGG_QUEEN:
        data_send[0] =1;
        data_send[1] =0;
        data_send[2] =0;
        data_send[3] =MOVE_CASLLINGG_QUEEN;
        break;
    case MOVE_PIECE:
        data_send[0] =1;
        data_send[1] =qfrom;
        data_send[2] =qto;
        data_send[3] =MOVE_PIECE;
        break;
    case MOVE_KILL:
        data_send[0] =1;
        data_send[1] =qfrom;
        data_send[2] =qto;
        data_send[3] =MOVE_KILL;
        break;
    case MOVE_PASSANT:
        data_send[0] =1;
        data_send[1] =qfrom;
        data_send[2] =qto;
        data_send[3] =MOVE_PASSANT;
        break;
    }
    int fd =open("/dev/i2c-1",O_RDWR | O_NONBLOCK);
    if(fd < 0){
        printf("I2C-Faile open i2c1\r\n");
        return -1;
    }
    if(ioctl(fd,I2C_SLAVE,ADDR_MAINBOARD) < 0){
        printf("I2C-Faile to set Address Mainboard");
        return -1;
    }
    if(write(fd,data_send,4)!= 4){
        printf("I2C-Faile write Move to Mainboard\r\n");
        return -1;
    }
     close(fd);
     return 0;
}

int Robot::calculatorMove(uint8_t board_before[64], uint8_t board_after[64])
{
    uint8_t qFrom=0,qTo=0,len_square_changed=0;
    uint8_t square_change[4];
    for(int i=0;i<64;i++){
        if(board_before[i] != board_after[i])   // phát hiện ô khác nhau
        {
            if(len_square_changed>3){ // nếu có trên 4 sự thay đổi trên bàn cờ thì move là lỗi
                RobochessApplication::instance()->mainWindow->sound.Sound_moveError();
                return -1;
            }
            square_change[len_square_changed] = i;
            len_square_changed++;
        }
    }

    if(len_square_changed < 2){   // nếu chỉ có 1 ô thay đổi thì move là error
        RobochessApplication::instance()->mainWindow->sound.Sound_moveError();
        return -1;
    }
    switch (len_square_changed) {
    case 2:                      // ăn quân hoặc di chuyển
        if(board_after[square_change[0]] == '_'){
            qFrom = square_change[0];
            qTo   = square_change[1];
        }else{
            qFrom = square_change[1];
            qTo   = square_change[0];
        }
        emit boardSensorMakeMove(convertSquareToMove(qFrom,qTo));
        qDebug()<<"You move:"<<convertSquareToMove(qFrom,qTo);
        return 0;
    case 3:                     // ăn tốt qua đường
        for(int k=0;k<3;k++){
            if(board_after[square_change[k]] != '_'){  // tìm qTo
                qTo = square_change[k];
            }
        }
        if((qTo+7)==square_change[0] || (qTo+7)==square_change[1] || (qTo+7)==square_change[2])
            qFrom = qTo+7;
        else if((qTo+9)==square_change[0] || (qTo+9)==square_change[1] || (qTo+9)==square_change[2])
            qFrom = qTo+9;
        emit boardSensorMakeMove(convertSquareToMove(qFrom,qTo));
        qDebug()<<"You move:passant-"<<convertSquareToMove(qFrom,qTo);
        return 0;
    case 4:                     // nhập thành
        int isTrue =0;
        for(int i=0;i<4;i++){
            if(square_change[i] ==60) isTrue++;
            if(square_change[i] ==61) isTrue++;
            if(square_change[i] ==62) isTrue++;
            if(square_change[i] ==63) isTrue++;
//            qDebug()<<"square_change:"<<square_change[i];
        }
        if(isTrue==4) {         // nhập thành cánh vua
            emit boardSensorMakeMove(convertSquareToMove(4,6));
            qDebug()<<"You move:Castling King";
            return 0;
        }else {
            isTrue =0;
            for(int i=0;i<4;i++){
                if(square_change[i] ==56) isTrue++;
                if(square_change[i] ==58) isTrue++;
                if(square_change[i] ==59) isTrue++;
                if(square_change[i] ==60) isTrue++;
            }
            if(isTrue==4){      // nhập thành cánh hậu
                emit boardSensorMakeMove(convertSquareToMove(4,1));
                qDebug()<<"You move: castling Queen";
                return 0;
            }
        }
    }
    return -1;
}

void Robot::printBoard(uint8_t dat[])
{
    printf("\f");
    for (int y = 0; y < 8; y++) {
       for (int x = 0; x < 8; x++) {
           printf("%c ",dat[y*8 +x]);
       }
       printf(" %d\r\n",(8 - y));
    }
    printf("\r\na b c d e f g h\r\n");
}

void Robot::requestMakeMove()
{
    qDebug()<<"You turn Move:";
//    RobochessApplication::instance()->mainWindow->m_gameViewer->boardScene()->number_piece_error = 5;
    step_getHumanMove = 1;
}

void Robot::boardMoveError()
{
    qDebug()<<"move Error";
    isHumanEnter=false;
    step_getHumanMove = 2;
}

void Robot::timeloop()
{
    /*  BEGIN DETECT PIECE ERROR    */
    readSensorBoard();
    emit onPieceError(data_board);
    taskCheckingPiecesPosition();
    taskStatus();
    taskArmMove();

    taskGetHumanMove();
}

void Robot::updateWifiSignal()
{
    QProcess process;
    int level=0;
    process.start("iw dev wlan0 link",QIODevice::ReadOnly);
    process.waitForFinished(1000);
    QString data = process.readAllStandardOutput();
    int index = data.indexOf("signal");
    QString s_signal =data.mid(index+9,3);
    int signal = s_signal.toInt();
    if(signal <=40)
        level = 4;
    else if(signal >40 && signal <=55)
        level = 3;
    else if(signal >55 && signal <=65)
        level = 2;
    else if(signal >65 && signal <=70)
        level = 1;
    else if(signal >70)
        level =0;
    RobochessApplication::instance()->info_wifi = level;
//    qDebug()<<"signal:"<<signal<<" level:"<<level;
    process.deleteLater();
}

void Robot::taskGetHumanMove()      // task này sẽ không phát ra lỗi vị trí pieces
{
    if(step_getHumanMove == 1){     // bước 1: kiểm tra xem vị trí pieces đã đúng chưa
        for(int i=0;i<64;i++){
            board_human_before[i] = data_board[i];  // lưu vị trí board trước khi di chuyển
        }
        RobochessApplication::instance()->mainWindow->sound.Sound_playerTurn(); // thông báo đến lượt người chơi di chuyển
        step_getHumanMove = 2;
    }else if(step_getHumanMove == 2){   // bước 2: chờ người dùng nhấn Enter (IR)
        if(isHumanEnter)    // khi người dùng bấm nút Enter
        {
            for(int i=0;i<64;i++){
               board_human_after[i] = data_board[i];  // lưu vị trí board sau khi di chuyển
            }
            if(calculatorMove(board_human_before,board_human_after) == 0) // tinh toan va tao nuoc di chuyen
            {
                qDebug("human move frist check ok");
                step_getHumanMove = 0;
            }else{
                 qDebug("move error! try again");
            }
            isHumanEnter = false;
        }
    }
}

void Robot::taskCheckingPiecesPosition()
{
    if(is_checking_piece_position){
        if(RobochessApplication::instance()->mainWindow->
            m_gameViewer->boardScene()->number_piece_error ==0){ // nếu m_gameViewer chưa khoi tao no sẽ gây lỗi
            qDebug()<<"pieces position ok";
            is_checking_piece_position = false;
            if(tik_task_checking_pieces==0){    // nếu checking lần đầu đã OK thì ko làm gì cả
                emit onCheckedBoardPieces();
                return;
            }else{                              // nếu phải chỉnh lại pieces thì phát thông báo "OK" sau khi chỉnh
                RobochessApplication::instance()->mainWindow->sound.Sound_ok();
                emit onCheckedBoardPieces();
            }
        }else{                                  // nếu sai thì thông phát thông báo ra loa
            if(tik_task_checking_pieces % 100 ==0){ // phát thông báo mỗi 20s
                RobochessApplication::instance()->mainWindow->sound.Sound_positionError();
            }
            tik_task_checking_pieces++;
         }
    }
}

void Robot::taskStatus()
{
    tik_taskStatus++;
    if(tik_taskStatus==5){
        tik_taskStatus=0;
        updateWifiSignal();
        i2c_readInfoMainBoard();
    }
}

void Robot::taskArmMove()
{
    if(step_arm_move == 1){         // bước 1: có lệnh bắt đầu
        i2c_writeMoveMainBoard(engine_qFrom,engine_qTo,engine_movetype);
        step_arm_move = 2;
        tik_delay_move_finish = 0;
        tik_delay_edit_piece  = 0;
    }else if(step_arm_move == 2){   // bước 2: delay khoảng 5s trước khi kiểm tra robot đã di chuyển xong chưa
        tik_delay_move_finish++;
        if(tik_delay_move_finish >=25){
            tik_delay_move_finish=0;
            step_arm_move = 3;
            qDebug()<<"ARM waiting fo finish";
        }
    }else if(step_arm_move == 3){   // bước 3: chờ đến khi robot di chuyển xong
        if(RobochessApplication::instance()->info_arm_move_finish){
            step_arm_move = 0;
            emit armRobotMovedFinish();
            qDebug()<<"ARM move piece finish";
        }
    }

}

QString Robot::convertSquareToMove(int qFrom, int qTo)
{
    char cmove[4];
    QString move;
    cmove[0] = (qFrom % 8) + 97; // a -> h
    cmove[1] = (8-(qFrom / 8)) + 48; // 1 -> 8
    cmove[2] = (qTo % 8) + 97; // a -> h
    cmove[3] = (8-(qTo / 8)) + 48; // 1 -> 8
    move = QString::fromUtf8(cmove,4);
    return move;
}
/* Hàm được gọi khi nhấn nút IR */
void Robot::onButtonEnter()
{
    qDebug()<<"IR-ENTER";
    isHumanEnter = true;
}

void Robot::onButtonTest()
{
    qDebug()<<"onButtonTest";
    requestMakeMove();
}

void Robot::armRobotMove(QString mov)
{
    qDebug()<<"ARM-robot-move:"<<mov;
    int cmove[4];
    cmove[0] = mov.at(0).toLatin1() -96;
    cmove[1] = mov.at(1).toLatin1() -48;
    cmove[2] = mov.at(2).toLatin1() -96;
    cmove[3] = mov.at(3).toLatin1() -48;
    engine_qFrom = (8-cmove[1])*8 + cmove[0] -1;
    engine_qTo   = (8-cmove[3])*8 + cmove[2] -1;
    qDebug()<<"ARM-robot-move:qFrom="<<engine_qFrom<<" qTo="<<engine_qTo;
    // nhập thành (hiện chỉ hỗ trợ bên đen)
    if(data_board[engine_qFrom]=='k' && engine_qFrom==4){
        if(engine_qTo==6){    //nhập thành cánh vua
            engine_movetype =   MOVE_CASLLING_KING;
            step_arm_move =1;       // bật Flag để gửi lệnh move qua I2C.
            return;
        }else if(engine_qTo == 2){    //nhập thành cánh hậu
            engine_movetype =   MOVE_CASLLINGG_QUEEN;
            step_arm_move =1;       // bật Flag để gửi lệnh move qua I2C.
            return;
         }
     }
     //ăn tốt qua đường
     //nếu tốt đi chéo mà ô đích hiện không có quân cờ nào
     if(data_board[engine_qFrom]=='p' && data_board[engine_qTo]=='_' && (engine_qTo-engine_qFrom==7 || engine_qTo-engine_qFrom==9)){
           engine_movetype =   MOVE_PASSANT;
           step_arm_move =1;       // bật Flag để gửi lệnh move qua I2C.
           return;
      }
      if(data_board[engine_qTo] != '_'){
            engine_movetype = MOVE_KILL;
       }else{
            engine_movetype = MOVE_PIECE;
       }
      step_arm_move =1;       // bật Flag để gửi lệnh move qua I2C.
}

void Robot::checkingBoardPiece()
{
    is_checking_piece_position = true; // on Flag
    tik_task_checking_pieces =0;
}

void Robot::debugPrintboard()
{
    printBoard(data_board);
}
