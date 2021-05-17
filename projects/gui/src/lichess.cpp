#include "lichess.h"
#include <QDebug>

Lichess::Lichess()
{
    netmanager = new QNetworkAccessManager();
//    m_token     = setting.value("ONLINE-token").toString();
    m_token     = "rtNyYpS2tkjw8rTX";
    m_username  = setting.value("ONLINE-username").toString();
    connect(netmanager,&QNetworkAccessManager::finished,this,&Lichess::networkReplyInfo);
    getAccount();
    connect(this,&Lichess::onNewgame,this,&Lichess::startStreamGame);
    startStreamIncoming();
    connect(this,&Lichess::incomingEventGameStart,this,&Lichess::onGameStart);
    connect(this,&Lichess::incomingEventGameFinish,this,&Lichess::onGameFinish);
    connect(this,&Lichess::incomingEventChallenge,this,&Lichess::onChallenge);
    connect(this,&Lichess::incomingEventChallengeCanceled,this,&Lichess::onChallengeCanceled);
    connect(this,&Lichess::incomingEventChallengeDeclined,this,&Lichess::onChallengeDeclined);

    //connect stategame

    connect(this,&Lichess::stateGameNewGame,this,&Lichess::onStateNewGame);
//    connect(this,&Lichess::stateGameMoves,this,&Lichess::onStateMoves);
}

void Lichess::getAccount()
{
    QNetworkRequest request(QUrl("https://lichess.org/api/account"));
    QString bearer = "Bearer ";
    bearer.append(m_token);
    request.setRawHeader("Authorization",bearer.toUtf8());
    netmanager->get(request);
}

void Lichess::ChallengeTheAI(int level,int time_limit,int time_inc,QString variant,bool color)
{
    QNetworkRequest request(QUrl("https://lichess.org/api/challenge/ai"));
    QString bearer = "Bearer ";
    bearer.append(m_token);
    request.setRawHeader("Authorization",bearer.toUtf8());
    QUrlQuery postData;
    postData.addQueryItem("level",QString::number(level));
    postData.addQueryItem("clock.limit",QString::number(time_limit));
    postData.addQueryItem("clock.increment",QString::number(time_inc));
    if(color)
        postData.addQueryItem("color", "black");
    else
        postData.addQueryItem("color","white");
    postData.addQueryItem("variant",variant);
    reply_challengeAI = netmanager->post(request,postData.toString(QUrl::FullyEncoded).toUtf8());
    connect(reply_challengeAI, &QIODevice::readyRead, this, &Lichess::replyChallengeAI);
}

void Lichess::makeMove(QString move)
{
    QString url = "https://lichess.org/api/board/game/";
    url.append(m_idGame);
    url.append("/move/");
    url.append(move);
    qDebug()<<"UrlMakemove:"<<url;
    QNetworkRequest request(QUrl(url.toUtf8()));
    QString bearer = "Bearer ";
    bearer.append(m_token);
    request.setRawHeader("Authorization",bearer.toUtf8());
    QUrlQuery postData;
    postData.addQueryItem("offeringDraw","false");

    reply_makeMove = netmanager->post(request,postData.toString(QUrl::FullyEncoded).toUtf8());
    connect(reply_makeMove, &QIODevice::readyRead, this, &Lichess::replyMakeMove);
}
void Lichess::networkReplyInfo(QNetworkReply *reply)
{
    QString data = reply->readAll();
    QJsonDocument doc = QJsonDocument::fromJson(data.toUtf8());
    m_username = doc.operator[]("username").toString();
    qDebug()<<"username:"<<m_username;

}

void Lichess::replyStreamIncomingEvent()
{
    QString data = reply_streamIncomingEvent->readAll();
    if(data.length()>5){
        QJsonDocument doc=QJsonDocument::fromJson(data.toUtf8());

        QString type = doc.operator[]("type").toString();

        if(type == "gameStart")
        {
            QString id = doc.operator[]("game").operator[]("id").toString();
            emit incomingEventGameStart(id);
        }else if(type == "gameFinish")
        {
            QString id = doc.operator[]("game").operator[]("id").toString();
            emit incomingEventGameFinish(id);
        }else if(type == "challenge")
        {
            QString id = doc.operator[]("challenge").operator[]("id").toString();
            QString challenger_name     = doc.operator[]("challenge").operator[]("challenger").operator[]("name").toString();
            int challenger_rating   = doc.operator[]("challenge").operator[]("challenger").operator[]("rating").toInt();
            bool    challenger_online   = doc.operator[]("challenge").operator[]("challenger").operator[]("online").toBool();
            QString challenge_variant  = doc.operator[]("challenge").operator[]("variant").operator[]("key").toString();
            bool    challenge_rated    = doc.operator[]("challenge").operator[]("rated").toBool();
            QString challenge_timeControl_type = doc.operator[]("challenge").operator[]("timeControl").operator[]("type").toString();
            int challenge_timeControl_limit = doc.operator[]("challenge").operator[]("timeControl").operator[]("limit").toInt();
            int challenge_timeControl_increment = doc.operator[]("challenge").operator[]("timeControl").operator[]("increment").toInt();
            QString challenge_color = doc.operator[]("challenge").operator[]("color").toString();

            emit incomingEventChallenge(id,challenger_name,challenger_rating,challenger_online,challenge_variant,
                                        challenge_rated,challenge_timeControl_type,challenge_timeControl_limit,
                                        challenge_timeControl_increment,challenge_color);

        }else if(type == "challengeCanceled")
        {
            QString id = doc.operator[]("challenge").operator[]("id").toString();
            QString challenger_name     = doc.operator[]("challenge").operator[]("challenger").operator[]("name").toString();
            emit incomingEventChallengeCanceled(id,challenger_name);
        }else if(type == "challengeDeclined")
        {
            QString id = doc.operator[]("challenge").operator[]("id").toString();
            QString challenger_name     = doc.operator[]("challenge").operator[]("challenger").operator[]("name").toString();
            emit incomingEventChallengeDeclined(id,challenger_name);
        }




        QString gameId = doc.operator[]("game").operator[]("id").toString();  // get gameId from json
//        qDebug()<<"StreamState:"<<data;
        if(gameId.length() == 8){
            m_idGame = gameId;
            emit incomingEventGameStart(gameId);
        }
        emit onNewgame(gameId);
    }
}

void Lichess::replyStreamGame()
{
    LichessData m_game;

    QString data = reply_streamGame->readAll();
    if(data.length() > 5)
    {
        qDebug()<<"StreamGame:"<<data;
        QJsonDocument doc=QJsonDocument::fromJson(data.toUtf8());
        QString id     = doc.operator[]("id").toString();
//        qDebug()<<"ID:"<<id<<" length:"<<id.length();
        /* nếu streamGame là bắt đầu 1 Game mới */

        if(id.length()==8)
        {
            m_game.id = id;
            m_game.variant = doc.operator[]("variant").operator[]("key").toString();
            m_game.rated    = doc.operator[]("rated").toBool();
            /* nếu Black là engine */
            if(doc.operator[]("black").operator[]("aiLevel").toInt() > 0)
            {
                m_game.white_name =  m_game.white_name= doc.operator[]("white").operator[]("name").toString();
                m_game.white_rating= doc.operator[]("white").operator[]("rating").toInt();
                m_game.black_name = "stockFish";
                m_game.black_isEngine = true;
                m_game.white_isEngine = false;
                m_game.black_rating = doc.operator[]("black").operator[]("aiLevel").toInt(); // nếu là Engine thì rating là level

            }   /* nếu White là engine */
            else if(doc.operator[]("white").operator[]("aiLevel").toInt() > 0)
            {
                m_game.black_name =  m_game.white_name= doc.operator[]("black").operator[]("name").toString();
                m_game.black_rating= doc.operator[]("black").operator[]("rating").toInt();
                m_game.white_name = "stockFish";
                m_game.white_isEngine = true;
                m_game.black_isEngine = false;
                m_game.white_rating = doc.operator[]("white").operator[]("aiLevel").toInt(); // nếu là Engine thì rating là level

            }   /* nếu cả 2 đều không phải là Engine */
            else
            {
                m_game.white_name =  m_game.white_name= doc.operator[]("white").operator[]("name").toString();
                m_game.white_rating= doc.operator[]("white").operator[]("rating").toInt();
                m_game.black_name =  m_game.white_name= doc.operator[]("black").operator[]("name").toString();
                m_game.black_rating= doc.operator[]("black").operator[]("rating").toInt();
                m_game.black_isEngine = false;
                m_game.white_isEngine = false;
            }
        }else   /* nếu streamGame tiếp tục game đang diễn ra */
        {
            if(doc.operator[]("type")=="gameState")
            {
                LichessMoves lichessmoves;
                QString smoves = doc.operator[]("moves").toString();
                lichessmoves.moves = smoves.split(' ');
                lichessmoves.moves_length = lichessmoves.moves.length();
                lichessmoves.move = lichessmoves.moves.last();

                lichessmoves.wtime = doc.operator[]("wtime").toInt();
                lichessmoves.btime = doc.operator[]("btime").toInt();
                lichessmoves.wdraw = doc.operator[]("wdraw").toBool();
                lichessmoves.bdraw = doc.operator[]("bdraw").toBool();
                lichessmoves.status= doc.operator[]("status").toString();
                if(lichessmoves.status =="resign")
                {
                    if(doc.operator[]("winner").toString() == "white")
                    {
                        lichessmoves.gamestate = lichessmoves.STATE_WHITE_WIN;
                    }else
                    {
                        lichessmoves.gamestate = lichessmoves.STATE_BLACK_WIN;
                    }
                }
                Chess::Side side;
                side = Chess::Side::White;
                if(lichessmoves.move.compare("e1g1"))
                emit stateGameMoves(lichessmoves.move,side);
                return;
            }
        }
        qDebug("emit stateGamenewGame");
        emit stateGameNewGame(m_game);
    }
}

void Lichess::replyChallengeAI()
{
    QString data = reply_challengeAI->readAll();
    qDebug()<<"ChallengeAI:"<<data;

}

void Lichess::replyMakeMove()
{
    QString data = reply_makeMove->readAll();
    qDebug()<<"Reply-MakeMove:"<<data;
}

void Lichess::startStreamIncoming()
{
    QNetworkRequest request(QUrl("https://lichess.org/api/stream/event"));
    QString bearer = "Bearer ";
    bearer.append(m_token);
    request.setRawHeader("Authorization",bearer.toUtf8());
    reply_streamIncomingEvent = netmanager->get(request);
    connect(reply_streamIncomingEvent, &QIODevice::readyRead, this, &Lichess::replyStreamIncomingEvent);
}

void Lichess::startStreamGame(QString idGame)
{
    QString url = "https://lichess.org/api/board/game/stream/";
    if(idGame.length()<5) return;
    url.append(idGame);
    QNetworkRequest request(QUrl(url.toUtf8()));
    QString bearer = "Bearer ";
    bearer.append(m_token);
    request.setRawHeader("Authorization",bearer.toUtf8());
    reply_streamGame = netmanager->get(request);
    connect(reply_streamGame, &QIODevice::readyRead, this, &Lichess::replyStreamGame);
}

void Lichess::onGameStart(QString idGame)
{
    qDebug()<<"onGameStart:"<<idGame;
}

void Lichess::onGameFinish(QString idGame)
{
    qDebug()<<"onGameFinish"<<idGame;
}

void Lichess::onChallenge(QString idGame, QString challenger_name, int rating, bool isOnline, QString variant, bool rated, QString timeType, int time_limit, int time_increment, QString color)
{
    qDebug()<<"onChallenge:idgame:"<<idGame<<" Name:"<<challenger_name<<" rating:"<<rating<< "online"<<isOnline;
    qDebug()<<"variant:"<<variant<<"rated:"<<rated<<"timeType:"<<timeType<<" timeLimit:"<<time_limit<<" inc:"<<time_increment<<" Color:"<<color;
}

void Lichess::onChallengeCanceled(QString idGame, QString challenger_name)
{
    qDebug()<<"onChallengeCanceled:idGame:"<<idGame<<" name:"<<challenger_name;
}

void Lichess::onChallengeDeclined(QString idGame, QString challenger_name)
{
    qDebug()<<"onChallengeDeclined:idGame:"<<idGame<<" name:"<<challenger_name;
}

void Lichess::onStateNewGame(LichessData dat)
{
    qDebug()<<"stateNewGame:"<<"id:"<<dat.id<<"variant:"<<dat.variant<<"WhiteName:"<<dat.white_name<<"rating:"<<dat.white_rating;
    qDebug()<<"rated:"<<dat.rated<<"_WhiteisEngine:"<<dat.white_isEngine<<" _BlackEngine:"<<dat.black_isEngine;
    qDebug()<<"wtime:"<<dat.white_timeRemain<<" btime:"<<dat.black_timeReamin;
}

void Lichess::onStateMoves(LichessMoves mov)
{
    qDebug()<<"MOVES:"<<mov.moves<<"_wtime:"<<mov.wtime<<"_btime:"<<mov.btime<<"_wdraw:"<<mov.wdraw<<"_bdraw:"<<mov.bdraw;
    qDebug()<<"Moves:"<<mov.gamestate<<" length:"<<mov.moves_length<<"  move:"<<mov.move;
}
