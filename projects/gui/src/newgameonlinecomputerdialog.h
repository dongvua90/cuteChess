#ifndef NEWGAMEONLINECOMPUTERDIALOG_H
#define NEWGAMEONLINECOMPUTERDIALOG_H

#include <QDialog>
#include <QWidget>
#include <QSettings>
#include "chessgame.h"

class PlayerBuilder;
namespace Ui {
class BeginGameonlineComputer;
}
class NewGameOnlineComputerDialog : public QDialog
{
    Q_OBJECT
public:
    explicit NewGameOnlineComputerDialog(QWidget* parent = nullptr);
    /*! Creates and returns the ChessGame object. */
    ChessGame* createGame();
    void createLichessGame();
    int m_timer;
    int m_timeInc;
    bool m_color;
    int m_level;
    int m_variant;
    QString s_variant;
    bool humanIsWhite();
private slots:
    void on_slider_level_valueChanged(int value);

    void on_slider_timer_valueChanged(int value);

    void on_slider_timeInc_valueChanged(int value);

    void on_btn_playerColor_toggled(bool checked);

    void on_btn_ok_clicked();

    void on_btn_close_clicked();

private:

    Ui::BeginGameonlineComputer *ui;
    void readSetting();
    void writeSetting();
    void setTimer(int timer);
    void setTimeInc(int time);
    void setLevel(int value);
    int convertTimer();
    int convertTimeInc();
};

#endif // NEWGAMEONLINECOMPUTERDIALOG_H
