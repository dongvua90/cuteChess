#ifndef SOUND_H
#define SOUND_H

#include <QObject>
#include <QProcess>
class Sound : public QObject
{
    Q_OBJECT
public:
    explicit Sound(QObject *parent = nullptr);

public slots:
    void Sound_boardError();
    void Sound_drawGame();
    void Sound_initGame();
    void Sound_initPlayAsFriend();
    void Sound_initSystem();
    void Sound_lichessHelp();
    void Sound_moveError();
    void Sound_ok();
    void Sound_playerLose();
    void Sound_playerMoveError();
    void Sound_playerWin();
    void Sound_positionError();
    void Sound_welcome();
    void Sound_wifiLost();
    void Sound_batteryLow();
    void Sound_playerTurn();

};

#endif // SOUND_H
