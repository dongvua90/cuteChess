#ifndef CHALLENGEDIALOG_H
#define CHALLENGEDIALOG_H

#include <QDialog>
namespace Ui {
class Challenge;
}

class ChallengeDialog : public QDialog
{
    Q_OBJECT
public:
    ChallengeDialog(QWidget* parent);
public slots:
    void setChallenger(QString rival_name,QString info);
private slots:
    void on_btn_ok_clicked();

    void on_btn_decline_clicked();

private:
    Ui::Challenge *ui;
};

#endif // CHALLENGEDIALOG_H
