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

class PgnGame;
class Lichess   :public QObject
{
    Q_OBJECT
public:
    Lichess();
    void getAccount();
    LichessData lichessdata;


    enum GAME_STATE{GAME_STATE_ILDE,GAME_STATE_STARTING,GAME_STATE_RUNING,GAME_STATE_FINISH}gamestate;
    int global_time_limit; // in minute
    int global_time_inc;    // in second
    QString m_user_name;
    QString m_token;
public slots:
    void ChallengeTheAI(int level,int time_limit,int time_inc,QString variant,bool color);
    void ChallengeTheFriend(QString friend_name,bool rated, int time_limit,int time_inc,QString variant,bool color);
    void ChallengeTheWord(bool rated, int time_limit,int time_inc,QString variant,bool color);
    void acceptChallenge();
    void declineChallenge();
    void cancelChallenge();
    void makeMove(QString move);
    void resignGame();
    void drawGame();
    void makeRequestDraw();
    void abortGame();

signals:
    void usernameChanged(QString username);
    /* Khi bắt đầu luồng Stream incomingEvent thì những Game đang diễn ra
        hoặc lời thách đấu đang tồn tại cũng sẽ phát ra signal */
    /* Phát ra 1 tín hiệu khi 1 game đã kết thúc */
    void incomingEventGameFinish(QString idGame,bool is_white_winner,bool isDraw,QString status);

    void incomingEventChallenge(QString challener_name,QString info);
    /* người chơi hủy lời thách đấu trước đó với bạn */
    void incomingEventChallengeCanceled(QString idGame,QString challenger_name);
    /* Đối thủ từ chối lời thách thức từ bạn hoặc bạn từ chối đối thủ */
    void incomingEventChallengeDeclined(QString idGame,QString challenger_name);
    void onAbortGame(bool success);
    void onResignGame(bool success);

    void stateGameMoves(QString move);
    void moveError(QString note);
    // flip:nếu là true thì Rival chơi quân trắng, Player chơi quân đen.
    void lichessNewGame(QString Rival_name,bool revalIsComputer,
                        QString Player_name,
                        bool flip,QString moves);

private:
    QSettings setting;
    QString m_idGame;
    QString m_idChallenge;
    QString m_variant;
    bool    m_rated;
    // các biến để khởi tạo 1 game mới
    QString m_rival_name;
    bool    m_rival_is_computer;
    int     m_rival_elo;    // nếu Reval làComputer thì elo tương đương level
    int     m_rival_time_left;
    int     m_player_elo;
    int     m_player_time_left;
    bool    m_flip;
    QString m_initialFen;

    QNetworkAccessManager *netmanager;
    QNetworkAccessManager *netmanager_stream;
    QNetworkAccessManager *netmanager_move;
    QNetworkReply *reply;
    QNetworkReply *reply_getInfo;
    QNetworkReply *reply_streamGame;
    QNetworkReply *reply_streamIncomingEvent;
    QNetworkReply *reply_challengeAI;
    QNetworkReply *reply_challengeFriend;
    QNetworkReply *reply_challengeWord;
    QNetworkReply *reply_makeMove;
    QNetworkReply *reply_request;
    QNetworkReply *reply_abort;
    QNetworkReply *reply_resign;

private: signals:
    void onNewgame(QString idGame);

private slots:
    void replyGetInfo();
    void replyStreamIncomingEvent();
    void replyStreamGame();
    void replyChallengeAI();
    void replyChallengeFriend();
    void replyChallengeWord();
    void replyMakeMove();
    void replyAbort();
    void replyResign();
    void replyRequest();



    void startStreamIncoming();
    void startStreamGame(QString idGame);
};

#endif // LICHESS_H
