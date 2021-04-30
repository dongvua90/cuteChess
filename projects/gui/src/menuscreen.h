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
    MenuScreen();
 signals:
    void onNewgame();
    void onDraw();
    void onResign();
    void onSave();
    void onFlip();
    void onSettings();
private slots:
    void on_btn_hide_clicked();

    void on_btn_newgame_clicked();

    void on_btn_draw_clicked();

    void on_btn_resign_clicked();

    void on_btn_save_clicked();

    void on_btn_flip_clicked();

    void on_btn_settings_clicked();

private:
    Ui::Menuscreen *ui;
};

#endif // MENUSCREEN_H
