#include "virtualkeyboard.h"
#include "ui_virtualkeyboard.h"

VirtualKeyboard::VirtualKeyboard(QWidget *parent) :QDialog(parent)
  ,ui(new Ui::keyboard)
{
    ui->setupUi(this);
    move(0,0);
}

void VirtualKeyboard::setData(QString dat)
{
    data = dat;
    ui->inputText->setText(data);
}

void VirtualKeyboard::on_key_changeStyle_clicked()
{

}

void VirtualKeyboard::on_key_changeStyle_toggled(bool checked)
{
    if(checked)
    {
        ui->key_q->setText("Q");
        ui->key_w->setText("W");
        ui->key_e->setText("E");
        ui->key_r->setText("R");
        ui->key_t->setText("T");
        ui->key_y->setText("Y");
        ui->key_u->setText("U");
        ui->key_i->setText("I");
        ui->key_o->setText("O");
        ui->key_p->setText("P");
        ui->key_a->setText("A");
        ui->key_s->setText("S");
        ui->key_d->setText("D");
        ui->key_f->setText("F");
        ui->key_g->setText("G");
        ui->key_h->setText("H");
        ui->key_j->setText("J");
        ui->key_k->setText("K");
        ui->key_l->setText("L");
        ui->key_z->setText("Z");
        ui->key_x->setText("X");
        ui->key_c->setText("C");
        ui->key_v->setText("V");
        ui->key_b->setText("B");
        ui->key_n->setText("N");
        ui->key_m->setText("M");
        ui->key_changeStyle->setText("abc");
    }else {
        ui->key_q->setText("q");
        ui->key_w->setText("w");
        ui->key_e->setText("e");
        ui->key_r->setText("r");
        ui->key_t->setText("t");
        ui->key_y->setText("y");
        ui->key_u->setText("u");
        ui->key_i->setText("i");
        ui->key_o->setText("o");
        ui->key_p->setText("p");
        ui->key_a->setText("a");
        ui->key_s->setText("s");
        ui->key_d->setText("d");
        ui->key_f->setText("f");
        ui->key_g->setText("g");
        ui->key_h->setText("h");
        ui->key_j->setText("j");
        ui->key_k->setText("k");
        ui->key_l->setText("l");
        ui->key_z->setText("z");
        ui->key_x->setText("x");
        ui->key_c->setText("c");
        ui->key_v->setText("v");
        ui->key_b->setText("b");
        ui->key_n->setText("n");
        ui->key_m->setText("m");
        ui->key_changeStyle->setText("ABC");
    }
}

void VirtualKeyboard::addCharacter(QChar chr)
{
    data.append(chr);
    ui->inputText->setText(data);
}

void VirtualKeyboard::on_key_q_clicked()
{
    addCharacter(ui->key_q->text().at(0));
}

void VirtualKeyboard::on_key_w_clicked()
{
    addCharacter(ui->key_w->text().at(0));
}

void VirtualKeyboard::on_key_e_clicked()
{
    addCharacter(ui->key_e->text().at(0));
}

void VirtualKeyboard::on_key_r_clicked()
{
    addCharacter(ui->key_r->text().at(0));
}

void VirtualKeyboard::on_key_t_clicked()
{
    addCharacter(ui->key_t->text().at(0));
}

void VirtualKeyboard::on_key_y_clicked()
{
    addCharacter(ui->key_y->text().at(0));
}

void VirtualKeyboard::on_key_u_clicked()
{
    addCharacter(ui->key_u->text().at(0));
}

void VirtualKeyboard::on_key_i_clicked()
{
    addCharacter(ui->key_i->text().at(0));
}

void VirtualKeyboard::on_key_o_clicked()
{
    addCharacter(ui->key_o->text().at(0));
}

void VirtualKeyboard::on_key_p_clicked()
{
    addCharacter(ui->key_p->text().at(0));
}

void VirtualKeyboard::on_key_a_clicked()
{
    addCharacter(ui->key_a->text().at(0));
}

void VirtualKeyboard::on_key_s_clicked()
{
    addCharacter(ui->key_s->text().at(0));
}

void VirtualKeyboard::on_key_d_clicked()
{
    addCharacter(ui->key_d->text().at(0));
}

void VirtualKeyboard::on_key_f_clicked()
{
    addCharacter(ui->key_f->text().at(0));
}

void VirtualKeyboard::on_key_g_clicked()
{
    addCharacter(ui->key_g->text().at(0));
}

void VirtualKeyboard::on_key_h_clicked()
{
    addCharacter(ui->key_h->text().at(0));
}

void VirtualKeyboard::on_key_k_clicked()
{
    addCharacter(ui->key_k->text().at(0));
}

void VirtualKeyboard::on_key_j_clicked()
{
    addCharacter(ui->key_j->text().at(0));
}

void VirtualKeyboard::on_key_l_clicked()
{
    addCharacter(ui->key_l->text().at(0));
}

void VirtualKeyboard::on_key_z_clicked()
{
    addCharacter(ui->key_z->text().at(0));
}

void VirtualKeyboard::on_key_x_clicked()
{
    addCharacter(ui->key_x->text().at(0));
}

void VirtualKeyboard::on_key_c_clicked()
{
    addCharacter(ui->key_c->text().at(0));
}

void VirtualKeyboard::on_key_v_clicked()
{
    addCharacter(ui->key_v->text().at(0));
}

void VirtualKeyboard::on_key_b_clicked()
{
    addCharacter(ui->key_b->text().at(0));
}

void VirtualKeyboard::on_key_n_clicked()
{
    addCharacter(ui->key_n->text().at(0));
}

void VirtualKeyboard::on_key_m_clicked()
{
    addCharacter(ui->key_m->text().at(0));
}

void VirtualKeyboard::on_key_dauphay_clicked()
{
    addCharacter(',');
}

void VirtualKeyboard::on_key_space_clicked()
{
    addCharacter(' ');
}

void VirtualKeyboard::on_key_daucham_clicked()
{
    addCharacter('.');
}

void VirtualKeyboard::on_key_backpress_clicked()
{
    data.chop(1);
    ui->inputText->setText(data);
}

void VirtualKeyboard::on_key_enter_clicked()
{
    emit keyboarEnter(data);
}

void VirtualKeyboard::on_key_cancel_clicked()
{
    emit keyboardCancel();
}

void VirtualKeyboard::on_key_1_clicked()
{
    addCharacter('1');
}

void VirtualKeyboard::on_key_2_clicked()
{
    addCharacter('2');
}

void VirtualKeyboard::on_key_3_clicked()
{
    addCharacter('3');
}

void VirtualKeyboard::on_key_4_clicked()
{
    addCharacter('4');
}

void VirtualKeyboard::on_key_5_clicked()
{
    addCharacter('5');
}

void VirtualKeyboard::on_key_6_clicked()
{
    addCharacter('6');
}

void VirtualKeyboard::on_key_7_clicked()
{
    addCharacter('7');
}

void VirtualKeyboard::on_key_8_clicked()
{
    addCharacter('8');
}

void VirtualKeyboard::on_key_9_clicked()
{
    addCharacter('9');
}

void VirtualKeyboard::on_key_0_clicked()
{
    addCharacter('0');
}
