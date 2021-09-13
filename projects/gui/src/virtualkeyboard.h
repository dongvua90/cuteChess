#ifndef VIRTUALKEYBOARD_H
#define VIRTUALKEYBOARD_H
#include <QDialog>
namespace Ui {
class keyboard;
}

class VirtualKeyboard :public QDialog
{
    Q_OBJECT
public:
    VirtualKeyboard(QWidget *parent=nullptr);
    QString data;
    enum EDITING{INPUT_FRIEND,INPUT_TOKEN}editing;
public  slots:
    void setData(QString dat);
signals:
    void keyboarEnter(QString data);
    void keyboardCancel();
private slots:
    void on_key_changeStyle_clicked();

    void on_key_changeStyle_toggled(bool checked);

    void on_key_q_clicked();

    void on_key_w_clicked();

    void on_key_e_clicked();

    void on_key_r_clicked();

    void on_key_t_clicked();

    void on_key_y_clicked();

    void on_key_u_clicked();

    void on_key_i_clicked();

    void on_key_o_clicked();

    void on_key_p_clicked();

    void on_key_a_clicked();

    void on_key_s_clicked();

    void on_key_d_clicked();

    void on_key_f_clicked();

    void on_key_g_clicked();

    void on_key_h_clicked();

    void on_key_k_clicked();

    void on_key_j_clicked();

    void on_key_l_clicked();

    void on_key_z_clicked();

    void on_key_x_clicked();

    void on_key_c_clicked();

    void on_key_v_clicked();

    void on_key_b_clicked();

    void on_key_n_clicked();

    void on_key_m_clicked();

    void on_key_dauphay_clicked();

    void on_key_space_clicked();

    void on_key_daucham_clicked();

    void on_key_backpress_clicked();

    void on_key_enter_clicked();

    void on_key_cancel_clicked();

    void on_key_1_clicked();

    void on_key_2_clicked();

    void on_key_3_clicked();

    void on_key_4_clicked();

    void on_key_5_clicked();

    void on_key_6_clicked();

    void on_key_7_clicked();

    void on_key_8_clicked();

    void on_key_9_clicked();

    void on_key_0_clicked();

private:
    Ui::keyboard *ui;
    void addCharacter(QChar chr);
};

#endif // VIRTUALKEYBOARD_H
