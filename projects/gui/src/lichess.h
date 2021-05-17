#ifndef LICHESS_H
#define LICHESS_H
#include <QtNetwork/QNetworkReply>
#include <QtNetwork/QNetworkRequest>
#include <QtNetwork/QNetworkAccessManager>

#include <QJsonDocument>
#include <QUrlQuery>
#include <QIODevice>
#include <QSettings>
#include <QObject>
#include "lichessdata.h"
#include "board/side.h"


class Lichess   :public QObject
{
    Q_OBJECT
public:
    Lichess();
    void getAccount();
    LichessData lichessdata;



public slots:
    void ChallengeTheAI(int level,int time_limit,int time_inc,QString variant,bool color);
    void makeMove(QString move);

    void networkReplyInfo(QNetworkReply *reply);
    void replyStreamIncomingEvent();
    void replyStreamGame();
    void replyChallengeAI();
    void replyMakeMove();

    void startStreamIncoming();
    void startStreamGame(QString idGame);

    /* SLOT of incomingEvent */
    void onGameStart(QString idGame);
    void onGameFinish(QString idGame);
    void onChallenge(QString idGame,QString challenger_name,int rating,bool isOnline,QString variant,
                     bool rated,QString timeType,int time_limit,int time_increment,QString color);
    void onChallengeCanceled(QString idGame,QString challenger_name);
    void onChallengeDeclined(QString idGame,QString challenger_name);

    /* SLOT of stateGame */
    void onStateNewGame(LichessData dat);
    void onStateMoves(LichessMoves mov);


signals:
    void onNewgame(QString idGame);
    /* Khi bắt đầu luồng Stream incomingEvent thì những Game đang diễn ra
        hoặc lời thách đấu đang tồn tại cũng sẽ phát ra signal */

    /* Phát ra 1 signal khi có 1 game mới bắt đầu với thông số là idGame */
    void incomingEventGameStart(QString idGame);
    /* Phát ra 1 tín hiệu khi 1 game đã kết thúc */
    void incomingEventGameFinish(QString idGame);
    /* Phát ra 1 signal khi có người chơi nào đó gửi lời thách đấu bạn
        thông số là thông tin của người chơi:
        name:
        ELO:
    */
    void incomingEventChallenge(QString idGame,QString challenger_name,int rating,bool isOnline,QString variant,
                     bool rated,QString timeType,int time_limit,int time_increment,QString color);
    /* người chơi hủy lời thách đấu trước đó với bạn */
    void incomingEventChallengeCanceled(QString idGame,QString challenger_name);
    /* Đối thủ từ chối lời thách thức từ bạn hoặc bạn từ chối đối thủ */
    void incomingEventChallengeDeclined(QString idGame,QString challenger_name);

    /* stream game
        - phát ra 1 signal khi bắt đầu 1 game mới
        - phát ra 1 signal khi có 1 bên di chuyển */
    void stateGameNewGame(LichessData dat);
    void stateGameMoves(QString move, Chess::Side side);

private:
    QSettings setting;
    QString m_username;
    QString m_token;
    QString m_idGame;

    QNetworkAccessManager *netmanager;
    QNetworkReply *reply;
    QNetworkReply *reply_streamGame;
    QNetworkReply *reply_streamIncomingEvent;
    QNetworkReply *reply_challengeAI;
    QNetworkReply *reply_makeMove;


};

#endif // LICHESS_H
