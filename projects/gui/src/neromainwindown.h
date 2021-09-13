#ifndef NEROMAINWINDOWN_H
#define NEROMAINWINDOWN_H

#include <QWidget>
#include <chessgame.h>

namespace Chess {
    class Board;
    class Move;
}
namespace Ui {
class NeroMainwindown;
}

class NeroMainWindown : public QWidget
{
    Q_OBJECT
public:
     NeroMainWindown(ChessGame *game);

signals:

private slots:
    void onGameManagerFinished();
    void onGameStarted(ChessGame *game);
    void on_btn_start_clicked();

    void on_btn_move_clicked();

private:
    Ui::NeroMainwindown *ui;
    ChessGame *mygame;

};

#endif // NEROMAINWINDOWN_H
