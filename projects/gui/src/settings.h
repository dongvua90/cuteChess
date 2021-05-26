#ifndef SETTINGS_H
#define SETTINGS_H

#include <QDialog>
#include <QSettings>
#include "virtualkeyboard.h"
namespace Ui {
class DlgSettings;
}

class Settings : public QDialog
{
    Q_OBJECT
public:
    Settings(QWidget *parent);
    bool speed_mode;
    QString token;

private slots:
    void onKeyboardEnter(QString data);
    void onKeyboardCancel();
    void onUsernameChanged(QString username);
    void on_btn_close_clicked();

    void on_btn_speedMode_toggled(bool checked);

    void on_btn_sound_toggled(bool checked);

    void on_btn_wifi_scan_clicked();

    void on_btn_help_clicked();

    void on_btn_inputtoken_clicked();

private:
    Ui::DlgSettings *ui;
    QSettings *setting;
    VirtualKeyboard *vtKeyboard;
};

#endif // SETTINGS_H
