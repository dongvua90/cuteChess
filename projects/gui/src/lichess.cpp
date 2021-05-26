#include "lichess.h"
#include <QDebug>

Lichess::Lichess()
    :gamestate(GAME_STATE_ILDE)
{
    netmanager = new QNetworkAccessManager();
    netmanager_move = new QNetworkAccessManager();
    netmanager_stream = new QNetworkAccessManager();
//    m_token     = "rtNyYpS2tkjw8rTX";
    m_token       = setting.value("SETTING_TOKEN").toString();
    m_user_name  = setting.value("ONLINE-username").toString();
    getAccount();
    startStreamIncoming();
    connect(this,&Lichess::onNewgame,this,&Lichess::startStreamGame);
}

void Lichess::getAccount()
{
    QNetworkRequest request(QUrl("https://lichess.org/api/account"));
    QString bearer = "Bearer ";
    bearer.append(m_token);
    request.setRawHeader("Authorization",bearer.toUtf8());
    reply_getInfo = netmanager->get(request);
    connect(reply_getInfo, &QIODevice::readyRead, this, &Lichess::replyGetInfo);
}

void Lichess::ChallengeTheAI(int level,int time_limit,int time_inc,QString variant,bool color)
{
    QNetworkRequest request(QUrl("https://lichess.org/api/challenge/ai"));
    QString bearer = "Bearer ";
    bearer.append(m_token);
    request.setRawHeader("Authorization",bearer.toUtf8());
    request.setHeader(QNetworkRequest::ContentTypeHeader,
                      QStringLiteral("application/x-www-form-urlencoded"));
    QUrlQuery postData;
    postData.addQueryItem("level",QString::number(level));
    if(time_limit >0){
        postData.addQueryItem("clock.limit",QString::number(time_limit));
        postData.addQueryItem("clock.increment",QString::number(time_inc));
    }

    if(color)
        postData.addQueryItem("color", "black");
    else
        postData.addQueryItem("color","white");
    postData.addQueryItem("variant",variant);
    reply_challengeAI = netmanager->post(request,postData.toString(QUrl::FullyEncoded).toUtf8());
    connect(reply_challengeAI, &QIODevice::readyRead, this, &Lichess::replyChallengeAI);
}

void Lichess::ChallengeTheFriend(QString friend_name, bool rated, int time_limit, int time_inc, QString variant, bool color)
{
    QString url = "https://lichess.org/api/challenge/";
    url.append(friend_name);
    QNetworkRequest request(QUrl(url.toUtf8()));
    QString bearer = "Bearer ";
    bearer.append(m_token);
    request.setRawHeader("Authorization",bearer.toUtf8());
    request.setHeader(QNetworkRequest::ContentTypeHeader,
                      QStringLiteral("application/x-www-form-urlencoded"));
    QUrlQuery postData;
    if(rated)
        postData.addQueryItem("rated","true");
    else
        postData.addQueryItem("rated","false");

    if(time_limit >0){
        postData.addQueryItem("clock.limit",QString::number(time_limit));
        postData.addQueryItem("clock.increment",QString::number(time_inc));
    }

    if(color)
        postData.addQueryItem("color", "black");
    else
        postData.addQueryItem("color","white");
    postData.addQueryItem("variant",variant);
    reply_challengeFriend = netmanager->post(request,postData.toString(QUrl::FullyEncoded).toUtf8());
    connect(reply_challengeFriend, &QIODevice::readyRead, this, &Lichess::replyChallengeFriend);
}

void Lichess::ChallengeTheWord(bool rated, int time_limit, int time_inc, QString variant, bool color)
{
    QNetworkRequest request(QUrl("https://lichess.org/api/board/seek"));
    QString bearer = "Bearer ";
    bearer.append(m_token);
    request.setRawHeader("Authorization",bearer.toUtf8());
    request.setHeader(QNetworkRequest::ContentTypeHeader,
                      QStringLiteral("application/x-www-form-urlencoded"));
    QUrlQuery postData;
    if(rated)
        postData.addQueryItem("rated","true");
    else
        postData.addQueryItem("rated","false");

    if(time_limit >0){
        postData.addQueryItem("time",QString::number(time_limit));
        postData.addQueryItem("increment",QString::number(time_inc));
    }

    if(color)
        postData.addQueryItem("color", "black");
    else
        postData.addQueryItem("color","white");
    postData.addQueryItem("variant",variant);
    reply_challengeWord = netmanager->post(request,postData.toString(QUrl::FullyEncoded).toUtf8());
    connect(reply_challengeWord, &QIODevice::readyRead, this, &Lichess::replyChallengeWord);
}

void Lichess::acceptChallenge()
{
    QString url = "https://lichess.org/api/challenge/";
    url.append(m_idChallenge);
    url.append("/accept");
    QNetworkRequest request(QUrl(url.toUtf8()));
    QString bearer = "Bearer ";
    bearer.append(m_token);
    request.setRawHeader("Authorization",bearer.toUtf8());
    request.setHeader(QNetworkRequest::ContentTypeHeader,
                      QStringLiteral("application/x-www-form-urlencoded"));
    QUrlQuery postData;
    postData.addQueryItem("null","false");
    reply_request = netmanager->post(request,postData.toString(QUrl::FullyEncoded).toUtf8());
    connect(reply_request, &QIODevice::readyRead, this, &Lichess::replyRequest);
}

void Lichess::declineChallenge()
{
    QString url = "https://lichess.org/api/challenge/";
    url.append(m_idChallenge);
    url.append("/decline");
    QNetworkRequest request(QUrl(url.toUtf8()));
    QString bearer = "Bearer ";
    bearer.append(m_token);
    request.setRawHeader("Authorization",bearer.toUtf8());
    request.setHeader(QNetworkRequest::ContentTypeHeader,
                      QStringLiteral("application/x-www-form-urlencoded"));
    QUrlQuery postData;
    postData.addQueryItem("reason","generic");
    reply_request = netmanager->post(request,postData.toString(QUrl::FullyEncoded).toUtf8());
    connect(reply_request, &QIODevice::readyRead, this, &Lichess::replyRequest);
}

void Lichess::cancelChallenge()
{
    QString url = "https://lichess.org/api/challenge/";
    url.append(m_idChallenge);
    url.append("/cancel");
    QNetworkRequest request(QUrl(url.toUtf8()));
    QString bearer = "Bearer ";
    bearer.append(m_token);
    request.setRawHeader("Authorization",bearer.toUtf8());
    request.setHeader(QNetworkRequest::ContentTypeHeader,
                      QStringLiteral("application/x-www-form-urlencoded"));
    QUrlQuery postData;
    postData.addQueryItem("null","false");
    reply_request = netmanager->post(request,postData.toString(QUrl::FullyEncoded).toUtf8());
    connect(reply_request, &QIODevice::readyRead, this, &Lichess::replyRequest);
}

void Lichess::makeMove(QString move)
{
    qDebug()<<"lichess MakeMove:"<<move;
    QString url = "https://lichess.org/api/board/game/";
    url.append(m_idGame);
    url.append("/move/");
    url.append(move);
    QNetworkRequest request(QUrl(url.toUtf8()));
    QString bearer = "Bearer ";
    bearer.append(m_token);
    request.setRawHeader("Authorization",bearer.toUtf8());
    request.setHeader(QNetworkRequest::ContentTypeHeader,
                      QStringLiteral("application/x-www-form-urlencoded"));
    QUrlQuery postData;
    postData.addQueryItem("offeringDraw","false");

    reply_makeMove = netmanager_move->post(request,postData.toString(QUrl::FullyEncoded).toUtf8());
    connect(reply_makeMove, &QIODevice::readyRead, this, &Lichess::replyMakeMove);
}

void Lichess::resignGame()
{
    QString url = "https://lichess.org/api/board/game/";
    url.append(m_idGame);
    url.append("/resign");
//        qDebug()<<"UrlResign:"<<url;
    QNetworkRequest request(QUrl(url.toUtf8()));
    QString bearer = "Bearer ";
    bearer.append(m_token);
    request.setRawHeader("Authorization",bearer.toUtf8());
    request.setHeader(QNetworkRequest::ContentTypeHeader,
                      QStringLiteral("application/x-www-form-urlencoded"));
    QUrlQuery postData;
    postData.addQueryItem("null","false");
    reply_resign = netmanager->post(request,postData.toString(QUrl::FullyEncoded).toUtf8());
    connect(reply_resign, &QIODevice::readyRead, this, &Lichess::replyResign);
}

void Lichess::drawGame()
{
    QString url = "https://lichess.org/api/board/game/";
    url.append(m_idGame);
    url.append("/draw/true");
//        qDebug()<<"UrlDraw:"<<url;
    QNetworkRequest request(QUrl(url.toUtf8()));
    QString bearer = "Bearer ";
    bearer.append(m_token);
    request.setRawHeader("Authorization",bearer.toUtf8());
    request.setHeader(QNetworkRequest::ContentTypeHeader,
                      QStringLiteral("application/x-www-form-urlencoded"));
    QUrlQuery postData;
    postData.addQueryItem("null","false");
    reply_request = netmanager->post(request,postData.toString(QUrl::FullyEncoded).toUtf8());
    connect(reply_makeMove, &QIODevice::readyRead, this, &Lichess::replyRequest);
}

void Lichess::makeRequestDraw()
{
    qDebug()<<"lichess request Draw:";
    QString url = "https://lichess.org/api/board/game/";
    url.append(m_idGame);
    url.append("/move/e2e4");
    QNetworkRequest request(QUrl(url.toUtf8()));
    QString bearer = "Bearer ";
    bearer.append(m_token);
    request.setRawHeader("Authorization",bearer.toUtf8());
    request.setHeader(QNetworkRequest::ContentTypeHeader,
                      QStringLiteral("application/x-www-form-urlencoded"));
    QUrlQuery postData;
    postData.addQueryItem("offeringDraw","true");
    reply_makeMove = netmanager->post(request,postData.toString(QUrl::FullyEncoded).toUtf8());
    connect(reply_makeMove, &QIODevice::readyRead, this, &Lichess::replyMakeMove);
}

void Lichess::abortGame()
{
    QString url = "https://lichess.org/api/board/game/";
    url.append(m_idGame);
    url.append("/abort");
        qDebug()<<"UrlAbort:"<<url;
    QNetworkRequest request(QUrl(url.toUtf8()));
    QString bearer = "Bearer ";
    bearer.append(m_token);
    request.setRawHeader("Authorization",bearer.toUtf8());
    request.setHeader(QNetworkRequest::ContentTypeHeader,
                      QStringLiteral("application/x-www-form-urlencoded"));
    QUrlQuery postData;
    postData.addQueryItem("null","false");
    reply_abort = netmanager->post(request,postData.toString(QUrl::FullyEncoded).toUtf8());
    connect(reply_abort, &QIODevice::readyRead, this, &Lichess::replyAbort);
}

void Lichess::replyGetInfo()
{
    QString data = reply_getInfo->readAll();
    QJsonDocument doc = QJsonDocument::fromJson(data.toUtf8());
    m_user_name = doc.operator[]("username").toString();
    emit usernameChanged(m_user_name);
    qDebug()<<"username:"<<m_user_name;
}

void Lichess::replyStreamIncomingEvent()
{
    QString data = reply_streamIncomingEvent->readAll();
    if(data.length()>5){
        qDebug()<<"Incoming:"<<data;
        QJsonDocument doc=QJsonDocument::fromJson(data.toUtf8());

        QString type = doc.operator[]("type").toString();

        if(type == "gameStart")     // 1 game mới đã được tạo với thông số là idGame
        {
            QString id = doc.operator[]("game").operator[]("id").toString();
            if(gamestate == GAME_STATE_ILDE || gamestate == GAME_STATE_FINISH)
            {
                m_idGame  = id;
                gamestate = GAME_STATE_STARTING;           
            }
        }else if(type == "gameFinish")
        {
            QString id = doc.operator[]("game").operator[]("id").toString();
            gamestate = GAME_STATE_FINISH;
        }else if(type == "challenge")
        {
            QString challenger_name,info;
            m_idChallenge   = doc.operator[]("challenge").operator[]("id").toString();
            challenger_name     = doc.operator[]("challenge").operator[]("challenger").operator[]("name").toString();
            int challenger_rating   = doc.operator[]("challenge").operator[]("challenger").operator[]("rating").toInt();
//            bool    challenger_online   = doc.operator[]("challenge").operator[]("challenger").operator[]("online").toBool();
            QString challenge_variant  = doc.operator[]("challenge").operator[]("variant").operator[]("key").toString();
            bool    challenge_rated    = doc.operator[]("challenge").operator[]("rated").toBool();
            QString challenge_timeControl_type = doc.operator[]("challenge").operator[]("timeControl").operator[]("type").toString();
            int challenge_timeControl_limit = doc.operator[]("challenge").operator[]("timeControl").operator[]("limit").toInt();
            int challenge_timeControl_increment = doc.operator[]("challenge").operator[]("timeControl").operator[]("increment").toInt();
            QString challenge_color = doc.operator[]("challenge").operator[]("color").toString();


            if(challenger_name != m_user_name){ // nếu người thách đấu không phải chính mình
                qDebug()<<"ChallengeName:"<<challenger_name<<" m_username:"<<m_user_name;
                challenger_name.append(" (" +QString::number(challenger_rating)+")");
                info.append("ID:"+m_idChallenge+" Rated:"+QString::number(challenge_rated)+
                            " TimeLimit:"+QString::number(challenge_timeControl_limit)+
                            " TimeInc:"+QString::number(challenge_timeControl_increment)+
                            " Color:"+challenge_color);
                emit incomingEventChallenge(challenger_name,info);
            }

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

        if(gamestate == GAME_STATE_STARTING){
            QString gameId = doc.operator[]("game").operator[]("id").toString();  // get gameId from json
            emit onNewgame(gameId);
        }

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
            m_idGame = id;
            m_variant = doc.operator[]("variant").operator[]("key").toString();
            m_rated    = doc.operator[]("rated").toBool();

            /* nếu Black là engine */
            int _black_ai_level = doc.operator[]("black").operator[]("aiLevel").toInt();
            if(_black_ai_level > 0)
            {
                m_flip = false; // rival chơi quân đen
                m_rival_is_computer = true;
                m_player_elo = doc.operator[]("white").operator[]("rating").toInt();
                m_rival_name = "Stockfish Cấp độ "+QString::number(_black_ai_level);
                m_rival_elo = _black_ai_level;
                m_player_time_left = doc.operator[]("state").operator[]("wtime").toInt();
                m_rival_time_left  = doc.operator[]("state").operator[]("btime").toInt();
            }   /* nếu White là engine */
            else if(doc.operator[]("white").operator[]("aiLevel").toInt() > 0)
            {
                m_flip = true; // rival chơi quân trắng
                m_rival_is_computer = true;
                m_player_elo = doc.operator[]("black").operator[]("rating").toInt();
                m_rival_elo  = doc.operator[]("white").operator[]("aiLevel").toInt(); // nếu là Engine thì rating là level
                m_rival_name = "Stockfish Cấp độ "+QString::number(m_rival_elo);
                m_player_time_left = doc.operator[]("state").operator[]("btime").toInt();
                m_rival_time_left  = doc.operator[]("state").operator[]("wtime").toInt();
            }   /* nếu cả 2 đều không phải là Engine */
            else
            {
                m_rival_is_computer = false;
                QString white_name =  m_game.white_name= doc.operator[]("white").operator[]("name").toString();
                if(white_name == m_user_name) // nếu trùng tên thì player chơi quân trắng
                {
                    m_player_elo = doc.operator[]("white").operator[]("rating").toInt();
                    m_rival_elo  = doc.operator[]("black").operator[]("rating").toInt();
                    m_rival_name = doc.operator[]("black").operator[]("name").toString();
                    m_flip = false;
                    m_player_time_left = doc.operator[]("state").operator[]("wtime").toInt();
                    m_rival_time_left  = doc.operator[]("state").operator[]("btime").toInt();
                }else       // ko thì player chơi quân đen
                {
                    m_rival_elo  = doc.operator[]("white").operator[]("rating").toInt();
                    m_player_elo = doc.operator[]("black").operator[]("rating").toInt();
                    m_rival_name = doc.operator[]("white").operator[]("name").toString();
                    m_player_time_left = doc.operator[]("state").operator[]("btime").toInt();
                    m_rival_time_left  = doc.operator[]("state").operator[]("wtime").toInt();
                }
            }
            QString _rivalname;
            if(m_rival_is_computer == false){
                _rivalname = m_rival_name + " ("+ QString::number(m_rival_elo)+")";
            }else{
                _rivalname = m_rival_name;
            }
            QString moves = doc.operator[]("state").operator[]("moves").toString();
            QString status = doc.operator[]("state").operator[]("status").toString();
            QString winner = doc.operator[]("state").operator[]("winner").toString();
            if(status == "started" && winner.length()<3){
                emit lichessNewGame(_rivalname,m_rival_is_computer,
                       m_user_name+" ("+QString::number(m_player_elo)+")",m_flip,moves);
            }
        }else   /* nếu streamGame tiếp tục game đang diễn ra */
        {
            if(doc.operator[]("type")=="gameState")
            {
                bool isWhiteWin=false;
                QStringList moves = doc.operator[]("moves").toString().split(' ');
                QString move = moves.last();
                QString status= doc.operator[]("status").toString();
                if(move.length()>3)
                    emit stateGameMoves(move);

                if(status =="stalemate"){
                    emit incomingEventGameFinish(m_idGame,false,true,status);
                }else if(status =="mate" || status =="resign"){
                    if(doc.operator[]("winner").toString() =="white")
                        isWhiteWin = true;
                    else
                        isWhiteWin = false;
                    emit incomingEventGameFinish(m_idGame,isWhiteWin,false,status);
                }

            }
        }
    }
}

void Lichess::replyChallengeAI()
{
    QString data = reply_challengeAI->readAll();
    qDebug()<<"ChallengeAI:"<<data;

}

void Lichess::replyChallengeFriend()
{
    QString data = reply_challengeFriend->readAll();
    qDebug()<<"ChallengeFriend:"<<data;
    QJsonDocument doc=QJsonDocument::fromJson(data.toUtf8());
    m_idChallenge = doc.operator[]("challenge").operator[]("id").toString();

}

void Lichess::replyChallengeWord()
{
    QString data = reply_challengeWord->readAll();
    qDebug()<<"ChallengeWord:"<<data;
}

void Lichess::replyMakeMove()
{
    QString data = reply_makeMove->readAll();
//    qDebug()<<"Reply-MakeMove:"<<data;
    QJsonDocument doc=QJsonDocument::fromJson(data.toUtf8());
    bool moveok     = doc.operator[]("ok").toBool();
    if(moveok==false){
        QString note = doc.operator[]("error").toString();
        emit moveError(note);
    }
}

void Lichess::replyAbort()
{
    QString data = reply_abort->readAll();
    QJsonDocument doc=QJsonDocument::fromJson(data.toUtf8());
    bool success =  doc.operator[]("ok").toBool();
    emit onAbortGame(success);
}

void Lichess::replyResign()
{
    QString data = reply_resign->readAll();
    QJsonDocument doc=QJsonDocument::fromJson(data.toUtf8());
    bool success =  doc.operator[]("ok").toBool();
    emit onResignGame(success);
}

void Lichess::replyRequest()
{
    QString data = reply_request->readAll();
    qDebug()<<"Reply-Request:"<<data;
}

void Lichess::startStreamIncoming()
{
    qDebug("startStreamIncoming...");
    QNetworkRequest request(QUrl("https://lichess.org/api/stream/event"));
    QString bearer = "Bearer ";
    bearer.append(m_token);
    request.setRawHeader("Authorization",bearer.toUtf8());
    reply_streamIncomingEvent = netmanager_stream->get(request);
    connect(reply_streamIncomingEvent, &QIODevice::readyRead, this, &Lichess::replyStreamIncomingEvent);
}

void Lichess::startStreamGame(QString idGame)
{
    qDebug()<<"startStreamGame";
    QString url = "https://lichess.org/api/board/game/stream/";
    if(idGame.length()<5) return;
    url.append(idGame);
    QNetworkRequest request(QUrl(url.toUtf8()));
    QString bearer = "Bearer ";
    bearer.append(m_token);
    request.setRawHeader("Authorization",bearer.toUtf8());
    reply_streamGame = netmanager_stream->get(request);
    connect(reply_streamGame, &QIODevice::readyRead, this, &Lichess::replyStreamGame);
}


