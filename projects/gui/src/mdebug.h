#ifndef MDEBUG_H
#define MDEBUG_H

#include <QObject>
#include <QThread>
#include <QDebug>
#include <iostream>
#include <QString>
#include <QStringList>

class Mdebug : public QThread
{
    Q_OBJECT
public:
    explicit Mdebug(QObject *parent = nullptr);
    void run();
signals:
    void debugMovePiece(QString mov);
    void debugPrintBoard();
    void debugCheckingBoardPiece();
    void debugHumanGetMove();
    void debugTRick();
private:
    std::string line;
    QStringList sline;

};

#endif // MDEBUG_H
