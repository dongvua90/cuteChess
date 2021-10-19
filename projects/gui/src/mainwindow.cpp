
#include "mainwindow.h"
#include <QAction>
#include <QHBoxLayout>
#include <QMenu>
#include <QMenuBar>
#include <QToolBar>
#include <QDockWidget>
#include <QTreeView>
#include <QMessageBox>
#include <QFileDialog>
#include <QInputDialog>
#include <QClipboard>
#include <QWindow>
#include <QSettings>
#include "boardview/boardscene.h"
#include <chessgame.h>
#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
#include <QDesktopWidget>
#endif
#include <QSysInfo>

#include <board/boardfactory.h>
#include <chessgame.h>
#include <timecontrol.h>
#include <enginemanager.h>
#include <gamemanager.h>
#include <playerbuilder.h>
#include <chessplayer.h>
#include <humanbuilder.h>
#include <tournament.h>

#include "robochessapp.h"
#include "gameviewer.h"
#include "movelist.h"
#include "newgameofflinedlg.h"
#include "chessclock.h"
#include "gametabbar.h"
#include "boardview/boardscene.h"
#include <QPushButton>
#include <enginebuilder.h>
#include "board/result.h"
#include <QProgressDialog>


MainWindow::MainWindow()
{

    // game online return: khởi tạo timer dành cho khởi tạo lại game đang chơi dở
    timer_oldGameReturn = new QTimer();
    connect(timer_oldGameReturn,&QTimer::timeout,this,&MainWindow::oldGameReturn);
    /* kết nối với robot: */
    initUI();

    /* khi chơi offline thì GameViewer là đối tượng chính phát ra Signal humanMoveError
     * còn khi chơi online thì Class Lichess sẽ phát ra Signal humanMoveError   */
    connect(m_gameViewer->boardScene(),&BoardScene::humanMoveError,this,&MainWindow::onHumanMoveError);
    // kết nối Lichess->move với GameViewer->makeMove
    /* khi chơi online: move -> lichess.move | streamGameState -> moves
     * lichess->moves gửi signal tới GameViewer và  MainWindow */
    connect(RobochessApplication::instance()->lichess,&Lichess::stateGameMoves,m_gameViewer,&GameViewer::makemove);
    connect(RobochessApplication::instance()->lichess,&Lichess::stateGameMoves,this,&MainWindow::movesOnlineMake);
    connect(RobochessApplication::instance()->lichess,&Lichess::moveError,this,&MainWindow::onHumanMoveError);
    connect(RobochessApplication::instance()->lichess,&Lichess::incomingEventGameFinish,this,&MainWindow::onGameOnlineFinish);
    connect(RobochessApplication::instance()->lichess,&Lichess::lichessNewGame,this,&MainWindow::addGameOnline);
    connect(RobochessApplication::instance()->lichess,&Lichess::incomingEventChallenge,this,&MainWindow::onChallenger);
    connect(RobochessApplication::instance()->lichess,&Lichess::onAbortGame,this,&MainWindow::abortGameOnline);
    connect(RobochessApplication::instance()->lichess,&Lichess::onResignGame,this,&MainWindow::resignGameOnline);
    //edit
    robot = RobochessApplication::instance()->robot;

    /* kết nối signal robot  */
//    connect(this,&MainWindow::humanMoveError,robot,&Robot::boardMoveError); // phát tín hiệu khi nước đi bị lỗi
    connect(this,&MainWindow::armMove,robot,&Robot::armRobotMove); // phát lệnh di chuyển 1 nước đi
    connect(this,&MainWindow::requestMove,robot,&Robot::requestMakeMove);  // yêu cầu 1 nước đi
    connect(RobochessApplication::instance()->mythread,&ThreadIR::onHumanEnter,robot,&Robot::onButtonEnter);
    connect(RobochessApplication::instance()->mythread,&ThreadIR::onButtonTest,robot,&Robot::onButtonTest); //test

    mdebug = new Mdebug(this);
    mdebug->start(QThread::NormalPriority);
    connect(mdebug,&Mdebug::debugTRick,robot,&Robot::onButtonEnter);
    connect(mdebug,&Mdebug::debugMovePiece,this,&MainWindow::onMakeMove);
    connect(mdebug,&Mdebug::debugPrintBoard,robot,&Robot::debugPrintboard);
    connect(mdebug,&Mdebug::debugCheckingBoardPiece,robot,&Robot::checkingBoardPiece);
    connect(mdebug,&Mdebug::debugHumanGetMove,robot,&Robot::requestMakeMove);

    fristscreen->exec();

    /* Debug: sửa dụng 1 lineEdit và button để thay cho boardSensor vật lý để tạo 1 nước đi */
    connect(m_gameViewer,&GameViewer::debugMakeMove,this,&MainWindow::onMakeMove);
    connect(robot,&Robot::boardSensorMakeMove,this,&MainWindow::onMakeMove);
}

MainWindow::~MainWindow()
{
}

void MainWindow::setCurrentGame()
{
    for (int i = 0; i < 2; i++)
    {
        ChessPlayer* player(m_players[i]);
        if (player != nullptr)
        {
            disconnect(player, nullptr,
                       m_gameViewer->chessClock(Chess::Side::White), nullptr);
            disconnect(player, nullptr,
                       m_gameViewer->chessClock(Chess::Side::Black), nullptr);
        }
    }
    for (int i = 0; i < 2; i++)
    {
        Chess::Side side = Chess::Side::Type(i);
        ChessPlayer* player(main_game->player(side));
        m_players[i] = player;

        auto clock = m_gameViewer->chessClock(side);

        clock->stop();
        QString name = nameOnClock(player->name(), side);
        clock->setPlayerName(name);
        connect(player, SIGNAL(nameChanged(QString)),
            clock, SLOT(setPlayerName(QString)));

        clock->setInfiniteTime(player->timeControl()->isInfinite());

        if (player->state() == ChessPlayer::Thinking)
            clock->start(player->timeControl()->activeTimeLeft());
        else
            clock->setTime(player->timeControl()->timeLeft());

        connect(player, SIGNAL(startedThinking(int)),
                clock, SLOT(start(int)));

        connect(player, SIGNAL(stoppedThinking()),
            clock, SLOT(stop()));
        //edit
        if(humanIsWhite)
        {
            if(i==0){
                connect(player,&ChessPlayer::startedThinking,this,&MainWindow::onHumanTurn);
            }

        }else
        {
            if(i==1)
                connect(player,&ChessPlayer::startedThinking,this,&MainWindow::onHumanTurn);
        }
    }

    if (main_game->boardShouldBeFlipped())
    {
        m_gameViewer->boardScene()->flip();
    }
    m_gameViewer->setNotify(" ");
}

QString MainWindow::nameOnClock(const QString &name, Chess::Side side)
{
    QString text = name;
    bool displaySide = QSettings().value("ui/display_players_sides_on_clocks", false)
                      .toBool();
    if (displaySide)
        text.append(QString(" (%1)").arg(side.toString()));
    return text;
}

QString MainWindow::convertMoveToString(Chess::GenericMove mov)
{
    char move[4];
    move[0] = mov.sourceSquare().file()+97;
    move[1] = mov.sourceSquare().rank()+49;
    move[2] = mov.targetSquare().file()+97;
    move[3] = mov.targetSquare().rank()+49;
    return QString::fromUtf8(move,4);
}

void MainWindow::onMakeMove(QString move)
{
    qDebug()<<"onMakeMove:"<<move;
    if(typegame == TYPEGAME_OFFLINE)
    {
        m_gameViewer->makemove(move);
    }else if(typegame == TYPEGAME_COMPUTER)
    {
        RobochessApplication::instance()->lichess->makeMove(move);
    }

}

void MainWindow::onHumanTurn(int timeleft)
{
    if(typegame==TYPEGAME_OFFLINE){
        if(timeleft>0){
            auto clock = m_gameViewer->chessClock(Chess::Side::White);
            clock->stop();
        }
    }
    qDebug()<<"onHumanturn";
    robot->requestMakeMove();
}

void MainWindow::newGameOffline()
{
    main_game = fristscreen->dlg_newGameOffLine->createGame();
    if (!main_game)
    {
        QMessageBox::critical(this, tr("Could not initialize game"),
                      tr("The game could not be initialized "
                     "due to an invalid opening."));
        return;
    }
    PlayerBuilder* builders[2] = {
        fristscreen->dlg_newGameOffLine->createPlayerBuilder(Chess::Side::White),
        fristscreen->dlg_newGameOffLine->createPlayerBuilder(Chess::Side::Black)
    };

    if (builders[main_game->board()->sideToMove()]->isHuman())
        main_game->pause();
    connect(main_game, SIGNAL(initialized(ChessGame*)),
        this, SLOT(addGameOffline(ChessGame*)));
    connect(main_game, SIGNAL(startFailed(ChessGame*)),
        this, SLOT(onGameStartFailed(ChessGame*)));

    QList<ChessGame*> mlist = RobochessApplication::instance()->gameManager()->activeGames();
    if(mlist.count()>0) // nếu list_chessgame >0 thì tiến hành clearup (finish())
        RobochessApplication::instance()->gameManager()->finish();
    RobochessApplication::instance()->gameManager()->newGame(main_game,
           builders[Chess::Side::White], builders[Chess::Side::Black]);
    humanIsWhite = fristscreen->dlg_newGameOffLine->humanIsWhite();


}

void MainWindow::addGameOffline(ChessGame *game)
{
    disconnect(RobochessApplication::instance()->robot,&Robot::onPieceError,m_gameViewer->boardScene(),&BoardScene::setPieceError);
    typegame = TYPEGAME_OFFLINE;
    m_gameViewer->disconnectGame();
    m_gameViewer->mvlist->setGame(game);
    m_gameViewer->setGame(game);
    connect(game, &ChessGame::finished,this, &MainWindow::onGameOfflineFinished);
    setCurrentGame();
    // Signal khi bất cứ move nào được tạo.kết hợp humanEnabled(bool) để xác định move của engine
    connect(game, SIGNAL(moveMade(Chess::GenericMove, QString, QString)),
            this, SLOT(movesOfflineMake(Chess::GenericMove,QString, QString)));
    moves_length=0;
    menu->setEnableButton(false,false,true,false,true,false);
    connect(RobochessApplication::instance()->robot,&Robot::onPieceError,m_gameViewer->boardScene(),&BoardScene::setPieceError);
    robot->checkingBoardPiece();
    robot->mainboardResetSquareKill();
    onHumanTurn(0);
}

void MainWindow::movesOfflineMake(const Chess::GenericMove& mov,
                                  const QString& sanString,
                                  const QString& comment)
{
     qDebug()<<"MOVES:"<<convertMoveToString(mov)<<"san:"<<sanString<<" comment:"<<comment;
     moves_length++;
     // do hàm này được gọi mỗi khi có 1 nước đi nên ta cần tách lấy nước đi của ENGINE và gửi đến robot-move
     if(humanIsWhite){  // nếu người chơi chơi quân trắng
         if((moves_length % 2)==0){  // thì moves_length = 2,4,6,8... là nước đi của ENGINE
             qDebug()<<"Engine-move:"<<convertMoveToString(mov);
             auto clock = m_gameViewer->chessClock(Chess::Side::White);
             RobochessApplication::instance()->robot->armRobotMove(convertMoveToString(mov));

             if(main_game->isFinished()==false){
                 clock->start(clock->m_totalTime);
             }
         }else{ // nước đi của Engine
             m_gameViewer->setNotify("Đối thủ đã di chuyển: "+convertMoveToString(mov));
         }
     }else{ // nếu người chơi quân đen
         if((moves_length %2)==1){  // thì moves_length = 1,3,5,7,9... là nước đi của ENGINE
             qDebug()<<"Engine-move:"<<convertMoveToString(mov);
             main_game->pause();
             RobochessApplication::instance()->robot->armRobotMove(convertMoveToString(mov));
             main_game->resume();
         }else{ // nước đi của Engine
             m_gameViewer->setNotify("Đối thủ đã di chuyển: "+convertMoveToString(mov));
         }
     }
}

void MainWindow::onGameOfflineFinished(ChessGame *game, Chess::Result result)
{
    Q_UNUSED(game);
    qDebug()<<"onGameofflineFinish";
    menu->setEnableButton(true,false,false,false,true,false);
    if(result.type() == Chess::Result::Win){
        if(humanIsWhite && result.toShortString()=="1-0"){
            sdialog->showDialog("YOU WIN!","Confugation!you is Winner",5000);
            m_gameViewer->setNotify("Bạn đã Thắng! \r\n Xin chúc mừng");
            sound.Sound_playerWin();
        }
        else{
            sdialog->showDialog("YOU LOSED!","Losed!you is Losed",5000);
            m_gameViewer->setNotify("Bạn đã thua!");
            sound.Sound_playerLose();
        }

    }else if(result.type() == Chess::Result::Draw){
        sdialog->showDialog("DRAW GAME!","Game is Draw!",5000);
        sound.Sound_drawGame();
    }else if(result.type() == Chess::Result::Resignation  && humanIsWhite){
        sdialog->showDialog("YOU LOSED!","You is Resigned!",5000);
        m_gameViewer->setNotify("Bạn đã xin thua!");
        sound.Sound_playerLose();
    }else if(result.type() == Chess::Result::Timeout){
        sdialog->showDialog("YOU LOSED!","Game is Timeout!",5000);
        m_gameViewer->setNotify("Bạn đã thua do hết thời gian!");
        sound.Sound_playerLose();
    }else{
        sdialog->showDialog("GAME FINISHED!","game is finished",5000);
        m_gameViewer->setNotify("Ván cờ kết thúc!");
        sound.Sound_playerLose();
    }
}

void MainWindow::movesOnlineMake(QString mov)
{
    moves_length++;
    if(humanIsWhite)
    {
        if(moves_length%2==0){
//            qDebug()<<"ENGINE-MakeMove:"<<mov;
            m_gameViewer->setNotify("Đối thủ đã di chuyển: "+mov);
            emit armMove(mov);
        }
    }else{
        if(moves_length%2){
//            qDebug()<<"ENGINE-MakeMove:"<<mov;
            m_gameViewer->setNotify("Đối thủ đã di chuyển: "+mov);
            emit armMove(mov);
        }
    }
    if(moves_length >1 && moves_length<3){
        menu->setEnableButton(false,false,true,false,true,false);
    }
}

void MainWindow::onHumanMoveError(QString err)
{
    qDebug("ON-Human-Move-Error!!!");
    m_gameViewer->setNotify("Di chuyển bị lỗi!\r\n Thử lại!");
    sdialog->showDialog("MOVE ERROR!!!",err,2000);
    sound.Sound_moveError();
//    emit humanMoveError();
    moved_is_ok = false;
    onHumanTurn(0);
}

void MainWindow::onGameStartFailed(ChessGame* game)
{
    QMessageBox::critical(this, tr("Error: init Game"), game->errorString());
}

void MainWindow::onChallengeCancel()
{
    dlg_challenge_cancel->close();
    RobochessApplication::instance()->lichess->cancelChallenge();

}

void MainWindow::onChallenger(QString rival_name,QString info)
{
    qDebug()<<"onChallenger"<<rival_name<<" info:"<<info;
    dlg_challenge->setChallenger(rival_name,info);
    dlg_challenge->exec();
}

void MainWindow::onSettingDlg()
{
      fristscreen->dlg_settings->exec();
      fristscreen->setVisible(true);
}
void MainWindow::onMenuResignGame()
{
    if(typegame == TYPEGAME_OFFLINE)
    {
        menu->setEnableButton(true,false,false,false,true,false);
        Chess::Result resuft(Chess::Result::Resignation);
        m_gameViewer->setNotify("Bạn đã xin thua!");
        main_game->onResignation(resuft);
    }else if(typegame == TYPEGAME_COMPUTER)
    {
        RobochessApplication::instance()->lichess->resignGame();
    }
}

void MainWindow::onMenuDrawGame()
{
    if(typegame == TYPEGAME_OFFLINE)
    {
        Chess::Result resuft(Chess::Result::Draw);
        main_game->onResignation(resuft);
    }else if(typegame == TYPEGAME_COMPUTER)
    {
        RobochessApplication::instance()->lichess->makeRequestDraw();
        Chess::Result resuft(Chess::Result::Draw);
        main_game->onResignation(resuft);
    }
}

void MainWindow::onMenuAbortGame()
{
    if(typegame == TYPEGAME_COMPUTER)
    {
        RobochessApplication::instance()->lichess->abortGame();
    }
}

void MainWindow::onMenuNewgame()
{
    fristscreen->setVisible(true);
}

void MainWindow::initUI()
{
    setFixedSize(480,272);
    move(0,0);
    QVBoxLayout* mainLayout = new QVBoxLayout();    // tạo layer cha để chứa m_gameViewer
    m_gameViewer = new GameViewer();        // Tạo 1 đối tượng GameViewer. chứa bàn cờ, Clock, Name,menu... là đối tượng giao diện chính
    m_gameViewer->setContentsMargins(0,0,0,0);      // thiết lập Margin bằng 0
    //    for (int i = 0; i < 2; i++)                     // lặp 2 lần để add 2 Clock cho 2 bên
//    {
//        Chess::Side side = Chess::Side::Type(i);
//        m_gameViewer->chessClock(side)->setPlayerName(side.toString()); // đặt tên cho clock
//    }
    mainLayout->addWidget(m_gameViewer);
    mainLayout->setContentsMargins(0, 0, 0, 0);
/* gán con trỏ menu là menu trong m_gameViewer
 * kết nối menu (newgame,draw,resign,flip,settings,save) với các hàm thực thi */
    menu = m_gameViewer->getMenu();
    connect(menu,SIGNAL(onNewgame()),this,SLOT(onMenuNewgame()));
    connect(menu,SIGNAL(onFlip()),m_gameViewer->boardScene(),SLOT(flip()));
    connect(menu,SIGNAL(onResign()),this,SLOT(onMenuResignGame()));
    connect(menu,SIGNAL(onDraw()),this,SLOT(onMenuDrawGame()));
    connect(menu,SIGNAL(onAbort()),this,SLOT(onMenuAbortGame()));
/* thêm layout chính vào MainWindow */
    QWidget* mainWidget = new QWidget(this);
    mainWidget->setLayout(mainLayout);
    setCentralWidget(mainWidget);
    mainWidget->setContentsMargins(0,0,0,0);

/* khởi tạo screen đầu tiên (màn hình newgame)
Kết nối button của fristscreen với các SLOT trong MainWindow */
    fristscreen = new FirstScreen(this);
    connect(fristscreen,SIGNAL(onCpuGame()),this,SLOT(newGameOffline()));
    connect(fristscreen,SIGNAL(onGameonlineComputer()),this,SLOT(newGameOnlineAi()));
    connect(fristscreen,SIGNAL(onGameonlineFriend()),this,SLOT(newGameOnlineFriend()));

    // khởi tạo dialog
    dlg_challenge               = new ChallengeDialog(this);        // dialog tự động show khi có người chơi gửi lời thách đấu
    sdialog                     = new ShowDialog(this);             // dialog nhỏ hiện thông báo và tự động ẩn sau 1 khoảng thời gian
    dlg_challenge_cancel        = new QDialog(this);                // dialog hiện thông báo chờ đối thủ chấp nhận thách đấu khi đã gửi lời thách đấu
    QLabel *lb_intro = new QLabel("Đang chờ đối thủ chấp nhận thách đấu...");
    QPushButton *btn_cancel = new QPushButton("Cancel");
    QHBoxLayout *layout_aswer = new QHBoxLayout();
    layout_aswer->addWidget(lb_intro);
    layout_aswer->addWidget(btn_cancel);
    dlg_challenge_cancel->setLayout(layout_aswer);
    connect(btn_cancel,SIGNAL(clicked()),this,SLOT(onChallengeCancel())); // kết nối với hàm thực thi lệnh hủy thách đấu khi nhấn nút Cancel
}

void MainWindow::onGameOnlineFinish(QString idGame, bool is_white_winner, bool isDraw,QString status)
{
    Q_UNUSED(idGame);
    menu->setEnableButton(true,false,false,false,true,false);
    if(is_white_winner && humanIsWhite && isDraw==false){
        sdialog->showDialog("YOU WIN!!!",status,3000);
        m_gameViewer->setNotify("VICTORY - You is Winner ");
    }else if(isDraw==false){
        sdialog->showDialog("YOU LOSED!!!",status,3000);
        m_gameViewer->setNotify("DEFEAT - You is loser ");
    }else if(isDraw){
        sdialog->showDialog("DRAW-GAME!!!",status,3000);
        m_gameViewer->setNotify("DRAW-GAME");
    }else{
        sdialog->showDialog("DRAW-GAME",status,3000);
    }
}


void MainWindow::oldGameReturn()
{
    if(num_returnGame < moves_returnGame.length())
    {
        QString move = moves_returnGame.at(num_returnGame);
        num_returnGame++;
        m_gameViewer->makemove(move);
    }
    if(num_returnGame == moves_returnGame.length()){
        timer_oldGameReturn->stop();
    }
}

void MainWindow::abortGameOnline(bool success)
{
    if(success){
        m_gameViewer->setNotify("Game is Aborted!");
        menu->setEnableButton(true,false,false,false,true,false);
        m_gameViewer->disconnectGame();
        RobochessApplication::instance()->gameManager()->finish();
    }
    else{
        m_gameViewer->setNotify("Abort Game Error!");
    }

}

void MainWindow::resignGameOnline(bool success)
{
    if(success){
        menu->setEnableButton(true,false,false,false,false,true);
        Chess::Result resuft(Chess::Result::Resignation);
        main_game->onResignation(resuft);
    }
}

void MainWindow::newGameOnlineAi()
{
    humanIsWhite = fristscreen->dlg_newGameOnlineComputer->humanIsWhite();
    sdialog->showDialog("Waiting For finish","waiting for game init");
}

void MainWindow::addGameOnline(QString Rival_name, bool revalIsComputer,
           QString Player_name,bool flip, QString moves)
{
    Q_UNUSED(revalIsComputer);

    bool continues_game=false;
    if(moves.length()>3)
    {
        QMessageBox msgBox;
        msgBox.move(100,60);
        msgBox.setText("Tiếp tục Game đang chơi");
        msgBox.setInformativeText("Bạn có 1 game đang chơi. \r\n Bạn có muốn tiếp tục");
        msgBox.setStandardButtons(QMessageBox::Ok | QMessageBox::Cancel);
        msgBox.setDefaultButton(QMessageBox::Ok);
        if(msgBox.exec() == QMessageBox::Cancel){
            return;
        }
        continues_game = true;
    }

    dlg_challenge_cancel->close();
    fristscreen->setVisible(false);

    auto board = Chess::BoardFactory::create("standard");
      auto pgn = new PgnGame();

    pgn->setSite(QSettings().value("pgn/site").toString());
    main_game = new ChessGame(board, pgn);

    // add timer clock
    if(RobochessApplication::instance()->lichess->global_time_limit==0){
        main_game->setTimeControl(TimeControl("inf"));
    }else{
        QString stim=QString::number(
             RobochessApplication::instance()->lichess->global_time_limit)+":00+"+
             QString::number(RobochessApplication::instance()->lichess->global_time_inc);
        Chess::Side player_side,rival_side;
        if(flip){
            player_side = Chess::Side::Black;
            rival_side  = Chess::Side::White;
        }else{
           player_side  = Chess::Side::White;
           rival_side   =Chess::Side::Black;
        }
        main_game->setTimeControl(TimeControl(stim),player_side);
        main_game->setTimeControl(TimeControl(stim),rival_side);
    }
    main_game->pause();

    PlayerBuilder *player_builder;
    PlayerBuilder *rival_builder;
    player_builder = new HumanBuilder(Player_name);
    rival_builder  = new HumanBuilder(Rival_name);
    connect(main_game, SIGNAL(initialized(ChessGame*)),
        this, SLOT(addGameToViewer(ChessGame*)));
    connect(main_game, SIGNAL(startFailed(ChessGame*)),
        this, SLOT(onGameStartFailed(ChessGame*)));

    QList<ChessGame*> mlist = RobochessApplication::instance()->gameManager()->activeGames();
    if(mlist.count()>0) // nếu list_chessgame >0 thì tiến hành clearup (finish())
        RobochessApplication::instance()->gameManager()->finish();
    if(flip){
        RobochessApplication::instance()->gameManager()->newGame(main_game,rival_builder,player_builder);
    }else{
        RobochessApplication::instance()->gameManager()->newGame(main_game,player_builder,rival_builder);
    }
    menu->setEnableButton(false,false,true,true,true,false);
    qDebug()<<"humanIsWhite:"<<humanIsWhite;
    if(humanIsWhite)
        onHumanTurn(0);

    /* trường hợp xây dựng lại game đã tạo trước đó. cần khởi tạo lại các nước di chuyển */
    if(continues_game){
        moves_returnGame = moves.split(' ');
        num_returnGame=0;
        timer_oldGameReturn->start(700);
    }
}

void MainWindow::addGameToViewer(ChessGame *game)
{
    disconnect(RobochessApplication::instance()->robot,&Robot::onPieceError,m_gameViewer->boardScene(),&BoardScene::setPieceError);
    typegame = TYPEGAME_COMPUTER;
    m_gameViewer->disconnectGame();
    m_gameViewer->mvlist->setGame(game);
    m_gameViewer->setGame(game);
    setCurrentGame();
    moves_length=0;
    connect(RobochessApplication::instance()->robot,&Robot::onPieceError,m_gameViewer->boardScene(),&BoardScene::setPieceError);
}

void MainWindow::newGameOnlineFriend()
{
    dlg_challenge_cancel->show();
    humanIsWhite = fristscreen->dlg_newgameOnlineFriend->humanIsWhite();
}
