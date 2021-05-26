
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
    setFixedSize(580,272);
    move(0,0);
    // khởi tạo timer dành cho khởi tạo lại game đang chơi dở
    timer_oldGameReturn = new QTimer();
    connect(timer_oldGameReturn,&QTimer::timeout,this,&MainWindow::oldGameReturn);
    /* kết nối với robot: */
    robotdebug = new RobotDebug();
    connect(this,SIGNAL(humanMoveError()),robotdebug,SLOT(boardMoveError())); // phát tín hiệu khi nước đi bị lỗi
    connect(this,&MainWindow::armMove,robotdebug,&RobotDebug::armRobotMove); // phát lệnh di chuyển 1 nước đi
    connect(this,SIGNAL(requestMove()),robotdebug,SLOT(requestMakeMove()));  // yêu cầu 1 nước đi
    /* Tạo 1 đối tượng GameViewer. chứa bàn cờ, Clock, Name,menu... là đối tượng giao diện chính */
    m_gameViewer = new GameViewer();
    for (int i = 0; i < 2; i++)                     // lặp 2 lần để add 2 Clock cho 2 bên
	{
		Chess::Side side = Chess::Side::Type(i);
        m_gameViewer->chessClock(side)->setPlayerName(side.toString()); // đặt tên cho clock
	}
    m_gameViewer->setContentsMargins(0,0,0,0);      // thiết lập Margin bằng 0

	QVBoxLayout* mainLayout = new QVBoxLayout();
	mainLayout->addWidget(m_gameViewer);
	mainLayout->setContentsMargins(0, 0, 0, 0);
    menu = m_gameViewer->getMenu();
    /* kết nối menu (newgame,draw,resign,flip,settings,save) với các hàm thực thi */
    connect(menu,SIGNAL(onNewgame()),this,SLOT(onMenuNewgame()));
    connect(menu,SIGNAL(onFlip()),m_gameViewer->boardScene(),SLOT(flip()));
    connect(menu,SIGNAL(onResign()),this,SLOT(onMenuResignGame()));
    connect(menu,SIGNAL(onDraw()),this,SLOT(onMenuDrawGame()));
    connect(menu,SIGNAL(onAbort()),this,SLOT(onMenuAbortGame()));

    // khởi tạo dialog
    dlg_newGameOffLine          = new NewGameOfflineDialog();
    dlg_newGameOnlineComputer   = new NewGameOnlineComputerDialog(this);
    dlg_newgameOnlineFriend     = new NewgameOnlineFriendDlg(this);
    dlg_challenge               = new ChallengeDialog(this);        // dialog tự động show khi có người chơi gửi lời thách đấu
    sdialog                     = new ShowDialog(this);             // dialog nhỏ hiện thông báo và tự động ẩn sau 1 khoảng thời gian
    dlg_settings                = new Settings(this);               // dialog settings
    dlg_challenge_cancel        = new QDialog(this);                // dialog hiện thông báo chờ đối thủ chấp nhận thách đấu khi đã gửi lời thách đấu
    QLabel *lb_intro = new QLabel("Đang chờ đối thủ chấp nhận thách đấu...");
    QPushButton *btn_cancel = new QPushButton("Cancel");
    QHBoxLayout *layout_aswer = new QHBoxLayout();
    layout_aswer->addWidget(lb_intro);
    layout_aswer->addWidget(btn_cancel);
    dlg_challenge_cancel->setLayout(layout_aswer);
    connect(btn_cancel,SIGNAL(clicked()),this,SLOT(onChallengeCancel())); // kết nối với hàm thực thi lệnh hủy thách đấu khi nhấn nút Cancel
    /* thêm layout chính vào MainWindow */
	QWidget* mainWidget = new QWidget(this);
	mainWidget->setLayout(mainLayout);
    setCentralWidget(mainWidget);
    mainWidget->setContentsMargins(0,0,0,0);
    /* khởi tạo screen đầu tiên (màn hình newgame) */
    fristscreen = new FirstScreen(this);
    /* Kết nối button của fristscreen với các SLOT trong MainWindow */
    connect(fristscreen,SIGNAL(onCpuGame()),this,SLOT(newGameOffline()));
    connect(fristscreen,SIGNAL(onGameonlineComputer()),this,SLOT(newGameOnlineAi()));
    connect(fristscreen,SIGNAL(onGameonlineFriend()),this,SLOT(newGameOnlineFriend()));
    connect(fristscreen,SIGNAL(onSettings()),this,SLOT(onSettingDlg()));

    /* khi chơi offline thì GameViewer là đối tượng chính phát ra Signal humanMoveError
     * còn khi chơi online thì Class Lichess sẽ phát ra Signal humanMoveError   */
    connect(m_gameViewer->boardScene(),&BoardScene::humanMoveError,this,&MainWindow::onHumanMoveError);
    // kết nối Lichess->move với GameViewer->makeMove
    /* khi chơi online: move -> lichess.move | streamGameState -> moves
     * lichess->moves gửi signal tới GameViewer và  MainWindow */
    connect(RobochessApplication::instance()->lichess,&Lichess::stateGameMoves,m_gameViewer,&GameViewer::makemove);
    connect(RobochessApplication::instance()->lichess,&Lichess::stateGameMoves,this,&MainWindow::movesOnlineMake);
    connect(this,SIGNAL(humanTurn()),this,SLOT(onTestHumanTurn()));  // tổng hợp để chỉ đưa ra 1 signal ra output
    connect(RobochessApplication::instance()->lichess,&Lichess::moveError,this,&MainWindow::onHumanMoveError);
    connect(RobochessApplication::instance()->lichess,&Lichess::incomingEventGameFinish,this,&MainWindow::onGameOnlineFinish);
    connect(RobochessApplication::instance()->lichess,&Lichess::lichessNewGame,this,&MainWindow::addGameOnline);
    connect(RobochessApplication::instance()->lichess,&Lichess::incomingEventChallenge,this,&MainWindow::onChallenger);
    connect(RobochessApplication::instance()->lichess,&Lichess::onAbortGame,this,&MainWindow::abortGameOnline);
    connect(RobochessApplication::instance()->lichess,&Lichess::onResignGame,this,&MainWindow::resignGameOnline);
    fristscreen->exec();

    /* Debug: sửa dụng 1 lineEdit và button để thay cho boardSensor vật lý để tạo 1 nước đi */
    connect(m_gameViewer,&GameViewer::debugMakeMove,this,&MainWindow::onMakeMove);
}

MainWindow::~MainWindow()
{
}

void MainWindow::onTestHumanTurn()
{
    if(humanIsWhite)
    {
        if(moves_length%2==0){
//            qDebug()<<"Den luot ban di chuyen!"<<moves_length;
            m_gameViewer->setNotify(m_gameViewer->getNotify() + "\r\nĐến lượt bạn di chuyển");
            emit requestMove();
        }
    }else{
        if(moves_length%2){
//            qDebug()<<"Den luot ban di chuyen!"<<moves_length;
            m_gameViewer->setNotify(m_gameViewer->getNotify() + "\r\nĐến lượt bạn di chuyển");
            emit requestMove();
        }
    }

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
    return QString::fromUtf8(move);
}

void MainWindow::onMakeMove(QString move)
{
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
    Q_UNUSED(timeleft);
    emit humanTurn();
}

void MainWindow::newGameOffline()
{
    // show dialog chọn các thông số để bắt đầu 1 game
    if (dlg_newGameOffLine->exec() != QDialog::Accepted){   // nếu người dùng hủy lệnh thì show dialog fristscreen
         fristscreen->setVisible(true);
        return;
    }
    fristscreen->setVisible(false);
    main_game = dlg_newGameOffLine->createGame();
    if (!main_game)
    {
        QMessageBox::critical(this, tr("Could not initialize game"),
                      tr("The game could not be initialized "
                     "due to an invalid opening."));
        return;
    }
    PlayerBuilder* builders[2] = {
        dlg_newGameOffLine->createPlayerBuilder(Chess::Side::White),
        dlg_newGameOffLine->createPlayerBuilder(Chess::Side::Black)
    };

//    if (builders[main_game->board()->sideToMove()]->isHuman())
//        main_game->pause();
    connect(main_game, SIGNAL(initialized(ChessGame*)),
        this, SLOT(addGameOffline(ChessGame*)));
    connect(main_game, SIGNAL(startFailed(ChessGame*)),
        this, SLOT(onGameStartFailed(ChessGame*)));

    QList<ChessGame*> mlist = RobochessApplication::instance()->gameManager()->activeGames();
    if(mlist.count()>0) // nếu list_chessgame >0 thì tiến hành clearup (finish())
        RobochessApplication::instance()->gameManager()->finish();
    RobochessApplication::instance()->gameManager()->newGame(main_game,
           builders[Chess::Side::White], builders[Chess::Side::Black]);
    humanIsWhite = dlg_newGameOffLine->humanIsWhite();
}

void MainWindow::addGameOffline(ChessGame *game)
{
    typegame = TYPEGAME_OFFLINE;
    m_gameViewer->disconnectGame();
    m_gameViewer->mvlist->setGame(game);
    m_gameViewer->setGame(game);
    connect(game, &ChessGame::finished,this, &MainWindow::onGameOfflineFinished);
    setCurrentGame();
    // Signal khi bất cứ move nào được tạo.kết hợp humanEnabled(bool) để xác định move của engine
    connect(game, SIGNAL(moveMade(Chess::GenericMove, QString, QString)),
            this, SLOT(movesOfflineMake(Chess::GenericMove)));
    moves_length=0;
    menu->setEnableButton(false,false,true,false,true,false);
}

void MainWindow::movesOfflineMake(Chess::GenericMove mov)
{
    moves_length++;
    if(humanIsWhite)
    {
        if(moves_length%2==0){
//            qDebug()<<"ENGINE-MakeMove:"<<convertMoveToString(mov);
            m_gameViewer->setNotify("Đối thủ đã di chuyển: "+convertMoveToString(mov));
            emit armMove(convertMoveToString(mov));
        }
    }else{
        if(moves_length%2){
//            qDebug()<<"ENGINE-MakeMove:"<<convertMoveToString(mov);
            m_gameViewer->setNotify("Đối thủ đã di chuyển: "+convertMoveToString(mov));
            emit armMove(convertMoveToString(mov));
        }
    }


}

void MainWindow::onGameOfflineFinished(ChessGame *game, Chess::Result result)
{
    Q_UNUSED(game);
    menu->setEnableButton(true,false,false,false,true,false);
    if(result.type() == Chess::Result::Win){
        if(humanIsWhite && result.toShortString()=="1-0"){
            sdialog->showDialog("YOU WIN!","Confugation!you is Winner",5000);
            m_gameViewer->setNotify("Bạn đã Thắng! \r\n Xin chúc mừng");
        }
        else{
            sdialog->showDialog("YOU LOSED!","Losed!you is Losed",5000);
            m_gameViewer->setNotify("Bạn đã thua!");
        }

    }else if(result.type() == Chess::Result::Draw){
        sdialog->showDialog("DRAW GAME!","Game is Draw!",5000);
    }else if(result.type() == Chess::Result::Resignation  && humanIsWhite){
        sdialog->showDialog("YOU LOSED!","You is Resigned!",5000);
        m_gameViewer->setNotify("Bạn đã xin thua!");
    }else if(result.type() == Chess::Result::Timeout){
        sdialog->showDialog("YOU LOSED!","Game is Timeout!",5000);
        m_gameViewer->setNotify("Bạn đã thua do hết thời gian!");
    }else{
        sdialog->showDialog("GAME FINISHED!","game is finished",5000);
        m_gameViewer->setNotify("Ván cờ kết thúc!");
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
//    qDebug("ON-Human-Move-Error!!!");
    m_gameViewer->setNotify("Di chuyển bị lỗi!\r\n Thử lại!");
    sdialog->showDialog("MOVE ERROR!!!",err,2000);
    emit humanMoveError();
}

void MainWindow::onGameStartFailed(ChessGame* game)
{
    QMessageBox::critical(this, tr("Game Error"), game->errorString());
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
      dlg_settings->exec();
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
    // show dialog chọn các thông số để bắt đầu 1 game
    if (dlg_newGameOnlineComputer->exec() != QDialog::Accepted){   // nếu người dùng hủy lệnh thì show dialog fristscreen
         fristscreen->setVisible(true);
        return;
    }
    fristscreen->setVisible(false);
    humanIsWhite = dlg_newGameOnlineComputer->humanIsWhite();
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
        msgBox.move(200,272/2);
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
        timer_oldGameReturn->start(300);
    }
}

void MainWindow::addGameToViewer(ChessGame *game)
{
    typegame = TYPEGAME_COMPUTER;
    m_gameViewer->disconnectGame();
    m_gameViewer->mvlist->setGame(game);
    m_gameViewer->setGame(game);
    setCurrentGame();
    moves_length=0;
}

void MainWindow::newGameOnlineFriend()
{
    // show dialog chọn các thông số để bắt đầu 1 game
    if (dlg_newgameOnlineFriend->exec() != QDialog::Accepted){   // nếu người dùng hủy lệnh thì show dialog fristscreen
         fristscreen->setVisible(true);
        return;
    }
    dlg_challenge_cancel->show();
    humanIsWhite = dlg_newgameOnlineFriend->humanIsWhite();
//    fristscreen->setVisible(false);
}
