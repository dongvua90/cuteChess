#include "robot.h"
#include <QDebug>
#include "cutechessapp.h"

#define ADDR_BOARD 0x19
#define ADDR_MAINBOARD 0x10
#define IRCODE_ENTER 32798

Robot::Robot()
{
    loopTime = new QTimer(this);
    connect(loopTime,&QTimer::timeout,this,&Robot::timeloop);
    loopTime->start(100);
    engineMoveState = MOVE_NULL;
    human_turn = HUMAN_NULL;
    initIrFile();

    timeCheckError = new QTimer(this);
    connect(timeCheckError,&QTimer::timeout,this,&Robot::checkErrorBoard);

}

void Robot::makeMove(int qFrom, int qTo, Robot::MOVETYPE move_type)
{
    engine_qFrom = qFrom;
    engine_qTo   = qTo;
    engine_movetype =   move_type;
    engineMoveState = MOVE_BEGIN;         // bật Flag để gửi lệnh move qua I2C.
}

void Robot::setFenOrigin(QString fen)
{
    convertFenToArraybyte(fen);
    printBoard(data_fen);
}

void Robot::requestHumanMove()
{
    timeCheckError->start(200); // kiểm tra vị trí piece trên bàn cờ.
                                    // chỉ chuyển sang bước HumanMakeMove khi không còn lỗi.
}

void Robot::humanEnter()
{
    printf("humanEnter\r\n");
    isHumanEnter =true;
}

int Robot::readSensorBoard()
{
    int fd = open("/dev/i2c-1",O_RDWR | O_NONBLOCK);
    if(fd < 0){
        printf("Faile open /dev/i2c-1");
        return -1;
    }

    if(ioctl(fd,I2C_SLAVE,ADDR_BOARD) < 0)
    {
        printf("Faile to set Address I2c");
        return -1;
    }
    if(read(fd,data_board,64)!= 64){
        printf("read error\r\n");
        return -1;
    }
    close(fd);
    return 0;
}

int Robot::readInfoMainBoard()
{
    uint8_t data_rev[7];

    int fd =open("/dev/i2c-1",O_RDWR | O_NONBLOCK);
    if(fd < 0){
        printf("Error open /dev/i2c-1\r\n");
        return -1;
    }
    if(ioctl(fd,I2C_SLAVE,ADDR_MAINBOARD) < 0){
        printf("Faile set Address I2C\r\n");
        return -1;
    }
    if(read(fd,data_rev,7)!= 7){
        printf("write error\r\n");
        return -1;
    }
     close(fd);
     battery = (data_rev[5] << 8) +data_rev[4];
     battery_is_changer = data_rev[6] & 0b00000100;
     move_is_running	= data_rev[6] & 0b00001000;
     return 0;
}

int Robot::writeMoveMainBoard(uint8_t qfrom, uint8_t qto, Robot::MOVETYPE type)
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
    case MOVE_PASSANT: break;
    }
    int fd =open("/dev/i2c-1",O_RDWR | O_NONBLOCK);
    if(fd < 0){
        printf("open Device error\r\n");
        return -1;
    }
    if(ioctl(fd,I2C_SLAVE,ADDR_MAINBOARD) < 0){
        printf("Faile to set Address I2c");
        return -1;
    }
    if(write(fd,data_send,4)!= 4){
        printf("write error\r\n");
        return -1;
    }
     close(fd);
     return 0;
}

int Robot::initIrFile()
{
    /* config enable NEC IR */
    int fd = open("/sys/class/rc/rc0/protocols",O_WRONLY);
    write(fd,"+nec",4);
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
                emit onHumanMoveError();
                return -1;
            }
            square_change[len_square_changed] = i;
            len_square_changed++;
        }
    }

    if(len_square_changed < 2){   // nếu chỉ có 1 ô thay đổi thì move là erorr
        emit onHumanMoveError();
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
        emit onHumanMakeMove(qFrom,qTo,Robot::MOVE_PIECE);
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
        emit onHumanMakeMove(qFrom,qTo,Robot::MOVE_PASSANT);

    case 4:                     // nhập thành
        int isTrue =0;
        for(int i=0;i<4;i++){
            if(square_change[i] ==56) isTrue++;
            if(square_change[i] ==57) isTrue++;
            if(square_change[i] ==58) isTrue++;
            if(square_change[i] ==59) isTrue++;
        }
        if(isTrue==4) {         // nhập thành cánh vua
            emit onHumanMakeMove(59,56,Robot::MOVE_CASLLING_KING);
            return 0;
        }else {
            isTrue =0;
            for(int i=0;i<4;i++){
                if(square_change[i] ==59) isTrue++;
                if(square_change[i] ==60) isTrue++;
                if(square_change[i] ==61) isTrue++;
                if(square_change[i] ==63) isTrue++;
            }
            if(isTrue==4){      // nhập thành cánh hậu
                emit onHumanMakeMove(59,62,Robot::MOVE_CASLLINGG_QUEEN);
                return 0;
            }
        }
    }
    return 0;
}

void Robot::checkErrorBoard()
{
    uint8_t len_error=0,squareErr[64];
    for(int i=0;i<64;i++)
    {
        if(data_fen[i] != data_board[i])
        {
            len_error++;
            squareErr[len_error] =i;
        }
    }
    if(len_error>0)
    {
        for(int i=0;i<len_error;i++)
        {
//            qDebug()<<"SquaErr:"<<i<<"-"<<squareErr[i];
        }
    }else{
        timeCheckError->stop();
        human_turn = HUMAN_BEGIN;   // bật Flag
    }
}

void Robot::onCheckMoveIsError()
{
    qDebug("onCheckMoveError! try move");
    human_turn = HUMAN_WAITING;
}

void Robot::onCheckMoveIsOk()
{
    qDebug("onCheckMoveOk");
    isHumanEnter = false;
    human_turn = HUMAN_NULL;
}

void Robot::printBoard(uint8_t dat[])
{
    for (int y = 0; y < 8; y++) {
       for (int x = 0; x < 8; x++) {
           printf("%c ",dat[y*8 +x]);
       }
       printf(" %d\r\n",(8 - y));
    }
    printf("\r\na b c d e f g h\r\n");
}

void Robot::humanEnabled(bool isHuman)
{
    isHumanTurn = isHuman;
    qDebug()<<"humanEnable:"<<isHuman;
}

void Robot::robotMakeMove(const Chess::GenericMove& move)
{
    if(isHumanTurn == true) return;
        enum MOVETYPE movetype;
        qDebug()<<"onMoveMake:"<<move.sourceSquare().file()<<move.sourceSquare().rank()
               <<" to "<<move.targetSquare().file()<<move.targetSquare().rank();
        uint8_t qfrom,qto;
        qfrom = (7 - move.sourceSquare().rank())*8 + move.sourceSquare().file();
        qto   = (7 - move.targetSquare().rank())*8 + move.targetSquare().file();
        // nhập thành (hiện chỉ hỗ trợ bên đen)
        if((data_board[qfrom]=='k' && data_board[qto]=='r'))
        {
            if(qto==63){    //nhập thành cánh vua
                makeMove(qfrom,qto,MOVE_CASLLING_KING);
            }else if(qto == 55){    //nhập thành cánh hậu
                makeMove(qfrom,qto,MOVE_CASLLINGG_QUEEN);
            }
            return;
        }
        //ăn tốt qua đường
        if(data_board[qfrom]=='p' && data_board[qto]!='_')
        {
            makeMove(qfrom,qto,MOVE_PASSANT);
            return;
        }

        if(data_board[qto] != '_')
        {
            movetype = MOVE_KILL;
        }else{
            movetype = MOVE_PIECE;
        }
        makeMove(qfrom,qto,movetype);
}


void Robot::convertFenToArraybyte(QString fen)
{
    int num=0;
    int i=0;
    while(i<65)
    {
        uint8_t piece = fen.at(num).unicode();
        num++;
        if((piece > 65 && piece <83)||(piece > 97 && piece<115)) //nếu ký tự là đại diện cac quân cờ
        {
            data_fen[i] = piece;
            i++;
         }else if(piece >48 && piece < 57) // nếu piece là số từ 1->8
         {
             for(int j=0;j<(piece-48);j++)
             {
                 data_fen[i] = '_';
                 i++;
              }
         }
    }
}

void Robot::timeloop()
{
    /*  BEGIN DETECT PIECE ERROR    */
    readSensorBoard();
    emit onPieceError(data_board);
    /*  END DETECT PIECE ERROR    */

    /*  BEGIN ENGINE MAKE MOVE  */
    if(engineMoveState == MOVE_BEGIN)   //nếu có lệnh Move từ engine
    {
        engineMoveState = MOVE_STARTED;
        writeMoveMainBoard(engine_qFrom,engine_qTo,engine_movetype);
    }else if(engineMoveState == MOVE_STARTED){
        tik_checkfinish++;
        if(tik_checkfinish==10){
            engineMoveState=MOVE_RUNNING;
            tik_checkfinish=0;
        }
    }
    else if(engineMoveState ==MOVE_RUNNING)
    {
        readInfoMainBoard();            //get Flag move_is_running
        if(move_is_running == false){
            engineMoveState = MOVE_NULL;    // reset Flag
            emit onMoveFinish();
        }
    }
    /*  END ENGINE MAKE MOVE  */

    /*  BEGIN HUMAN MAKE MOVE   */
    if(human_turn == HUMAN_BEGIN)
    {
        qDebug()<<"Request move";
        for(int i=0;i<64;i++){
            board_human_before[i] = data_board[i];  // lưu vị trí board trước khi di chuyển
        }
        isHumanEnter = false;
        human_turn = HUMAN_WAITING;
    }else if(human_turn == HUMAN_WAITING)
    {
//        printf("human_waiting..\r\n");
        if(isHumanEnter){
            printf("accep\r\n");
            isHumanEnter=false;
            readSensorBoard();
            for(int i=0;i<64;i++){
               board_human_after[i] = data_board[i];  // lưu vị trí board sau khi di chuyển
            }
           if(calculatorMove(board_human_before,board_human_after) == -1)
           {
                 qDebug("move error! try again");
                human_turn = HUMAN_WAITING;
                isHumanEnter = false;
           }else{
               qDebug("move temp ok");
//                human_turn = HUMAN_MOVECHECKING;    //chuyển sang bước kiểm tra move có hợp lệ ko
           }

        }
    }
    /*  END HUMAN MAKE MOVE     */

    /*  BEGIN READ INFO */
    tik_readInfo++;
    if(tik_readInfo==10) //chu kỳ đọc 1s
    {
        tik_readInfo=0;
        readInfoMainBoard();
        bool isChanged =false;
        if(battery != battery_save){
            battery_save = battery;
            isChanged = true;
        }
        if(battery_is_changer != battery_is_changer_save){
            battery_is_changer_save = battery_is_changer;
            isChanged = true;
        }
        if(isChanged) emit onBatteryChanged(battery,battery_is_changer);
    }
    /*  END READ INFO */
}
