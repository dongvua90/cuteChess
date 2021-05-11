#include "menuscreen.h"
#include "ui_menu.h"

MenuScreen::MenuScreen(QWidget *parent):QDialog(parent)
  ,ui(new Ui::Menuscreen)
{
    ui->setupUi(this);
    setFixedSize(400,272);
}

void MenuScreen::on_btn_hide_clicked()
{
    close();
}

void MenuScreen::on_btn_newgame_clicked()
{
    emit onNewgame();
    close();
}

void MenuScreen::on_btn_draw_clicked()
{
    emit onDraw();
    close();
}

void MenuScreen::on_btn_resign_clicked()
{
    emit onResign();
    close();
}

void MenuScreen::on_btn_save_clicked()
{
    onSave();
    close();
}

void MenuScreen::on_btn_flip_clicked()
{
    emit onFlip();
    close();
}

void MenuScreen::on_btn_settings_clicked()
{
    emit onSettings();
    close();
}
