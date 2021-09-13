#include "sound.h"
#define SOUND_INITSYSTEM "aplay /data/initsystem.wav"

Sound::Sound(QObject *parent) : QObject(parent)
{

}

void Sound::Sound_boardError()
{
    QProcess process;
    process.start("aplay /data/boardError.wav",QIODevice::ReadOnly);
    process.waitForFinished(100000);
    process.deleteLater();
}

void Sound::Sound_drawGame()
{
    QProcess process;
    process.start("aplay /data/drawGame.wav",QIODevice::ReadOnly);
    process.waitForFinished(100000);
    process.deleteLater();
}

void Sound::Sound_initGame()
{
    QProcess process;
    process.start("aplay /data/initGame.wav",QIODevice::ReadOnly);
    process.waitForFinished(100000);
    process.deleteLater();
}

void Sound::Sound_initPlayAsFriend()
{
    QProcess process;
    process.start("aplay /data/initPlayAsFriend.wav",QIODevice::ReadOnly);
    process.waitForFinished(100000);
    process.deleteLater();
}

void Sound::Sound_initSystem()
{
    QProcess process;
    process.start("aplay /data/initSystem.wav",QIODevice::ReadOnly);
    process.waitForFinished(100000);
    process.deleteLater();
}

void Sound::Sound_lichessHelp()
{
    QProcess process;
    process.start("aplay /data/lichessHelp.wav",QIODevice::ReadOnly);
    process.waitForFinished(100000);
    process.deleteLater();
}

void Sound::Sound_moveError()
{
    QProcess process;
    process.start("aplay /data/moveError.wav",QIODevice::ReadOnly);
    process.waitForFinished(100000);
    process.deleteLater();
}

void Sound::Sound_ok()
{
    QProcess process;
    process.start("aplay /data/ok.wav",QIODevice::ReadOnly);
    process.waitForFinished(100000);
    process.deleteLater();
}

void Sound::Sound_playerLose()
{
    QProcess process;
    process.start("aplay /data/playerLose.wav",QIODevice::ReadOnly);
    process.waitForFinished(100000);
    process.deleteLater();
}

void Sound::Sound_playerMoveError()
{
    QProcess process;
    process.start("aplay /data/playerMoveError.wav",QIODevice::ReadOnly);
    process.waitForFinished(100000);
    process.deleteLater();
}

void Sound::Sound_playerWin()
{
    QProcess process;
    process.start("aplay /data/playerWin.wav",QIODevice::ReadOnly);
    process.waitForFinished(100000);
    process.deleteLater();
}

void Sound::Sound_positionError()
{
    QProcess process;
    process.start("aplay /data/positionError.wav",QIODevice::ReadOnly);
    process.waitForFinished(100000);
    process.deleteLater();
}

void Sound::Sound_welcome()
{
    QProcess process;
    process.start("aplay /data/welcome.wav",QIODevice::ReadOnly);
    process.waitForFinished(100000);
    process.deleteLater();
}

void Sound::Sound_wifiLost()
{
    QProcess process;
    process.start("aplay /data/wifiLost.wav",QIODevice::ReadOnly);
    process.waitForFinished(100000);
    process.deleteLater();
}

void Sound::Sound_batteryLow()
{
    QProcess process;
    process.start("aplay /data/batteryLow.wav",QIODevice::ReadOnly);
    process.waitForFinished(100000);
    process.deleteLater();
}

void Sound::Sound_playerTurn()
{
    QProcess process;
    process.start("aplay /data/playerTurn.wav",QIODevice::ReadOnly);
    process.waitForFinished(100000);
    process.deleteLater();
}
