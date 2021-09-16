#include "mdebug.h"


Mdebug::Mdebug(QObject *parent) :QThread(parent)
{

}

void Mdebug::run()
{
    qDebug("debug start\r\n");
    while(1){
        std::getline( std::cin, line );
        qDebug()<<"ok";
        sline = QString::fromStdString(line).split(" ");
        if(sline.at(0).compare("movepiece")==0){
            if(sline.size() < 3){
                qDebug()<<"Faile! CMD: movepiece qfrom qto option";
                break;
            }
            int qfrom   = sline.at(1).toInt();
            int qto     = sline.at(2).toInt();
            int option;
            if(sline.at(3).compare("kill")==0)
                    option =1;
                else if(sline.at(3).compare("castlingking")==0)
                    option =2;
                else if(sline.at(3).compare("castlingqueen")==0)
                    option =3;
                else if(sline.at(3).compare("passant")==0)
                    option = 4;
                else option =0;
               qDebug()<<"Move Piece from "<<qfrom<< "to" <<qto<<"option:"<<option;
               emit debugTRick();
               qDebug()<<"triked";
        }else if(sline.at(0).compare("move")==0){               // Move Piece
            if(sline.size()<2){
                qDebug()<<"Fail!";
                break;
            }
            qDebug()<<"CMD: move";
            emit debugMovePiece(sline.at(1));
        }else if(sline.at(0).compare("printboard")==0){
            emit debugPrintBoard();
            qDebug()<<"emited";
        }else if(sline.at(0).compare("checkingpieces")==0){
            emit debugCheckingBoardPiece();
            qDebug()<<"emited";
        }else if(sline.at(0).compare("getmove")==0){
            emit debugHumanGetMove();
            qDebug()<<"emited";
        }
    }
}
