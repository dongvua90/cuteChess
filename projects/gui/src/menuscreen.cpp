#include "menuscreen.h"
#include "ui_menu.h"

MenuScreen::MenuScreen(QWidget *parent):QDialog(parent)
  ,ui(new Ui::Menuscreen)
{
    ui->setupUi(this);
    setFixedSize(400,272);
    ui->btn_hide->setIcon(QIcon(":/icons/toolbutton/next_16x16"));
}

void MenuScreen::setEnableButton(bool enable_newgame, bool enable_draw, bool enable_resign, bool enable_abort, bool enable_flip, bool enable_settings)
{
   ui->btn_newgame->setDisabled(!enable_newgame);
   ui->btn_draw->setDisabled(!enable_draw);
   ui->btn_resign->setDisabled(!enable_resign);
   ui->btn_abort->setDisabled(!enable_abort);
   ui->btn_flip->setDisabled(!enable_flip);
   ui->btn_settings->setDisabled(!enable_settings);
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

void MenuScreen::on_btn_abort_clicked()
{
    emit onAbort();
    close();
}
