#ifndef MENUSCREEN_H
#define MENUSCREEN_H

#include <QDialog>

namespace Ui {
class Menuscreen;
}

class MenuScreen : public QDialog
{
    Q_OBJECT
public:
    MenuScreen(QWidget *parent);
    void setEnableButton(bool enable_newgame,bool enable_draw,bool enable_resign,
                         bool enable_abort,bool enable_flip,bool enable_settings);
 signals:
    void onNewgame();
    void onDraw();
    void onResign();
    void onAbort();
    void onFlip();
    void onSettings();
private slots:
    void on_btn_hide_clicked();

    void on_btn_newgame_clicked();

    void on_btn_draw_clicked();

    void on_btn_resign_clicked();

    void on_btn_flip_clicked();

    void on_btn_settings_clicked();

    void on_btn_abort_clicked();

private:
    Ui::Menuscreen *ui;
};

#endif // MENUSCREEN_H
