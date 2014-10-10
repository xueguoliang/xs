#include "creg.h"


CReg::CReg(QWidget *parent) :
    QDialog(parent)
{
    QGridLayout* input = new QGridLayout;
    input->addWidget(new QLabel("Username:"), 0, 0);
    input->addWidget(_username = new QLineEdit, 0, 1);
    input->addWidget(new QLabel("Password:"), 1, 0);
    input->addWidget(_password = new QLineEdit, 1, 1);
    input->addWidget(new QLabel("Password Again:"), 2, 0);
    input->addWidget(_password2 = new QLineEdit, 2, 1);

    QHBoxLayout* hbox = new QHBoxLayout;
    QPushButton* button;
    hbox->addStretch(1);
    hbox->addWidget(button = new QPushButton("Reg"));

    QVBoxLayout*  center = new QVBoxLayout;
    center->addLayout(input);
    center->addLayout(hbox);

    QGridLayout* layout = new QGridLayout(this);
    layout->addLayout(center, 1, 1);
    layout->setRowStretch(0, 1);
    layout->setRowStretch(2, 1);
    layout->setColumnStretch(0, 1);
    layout->setColumnStretch(2, 1);

    _password->setEchoMode(QLineEdit::Password);
    _password2->setEchoMode(QLineEdit::Password);

    connect(button, SIGNAL(clicked()), this, SLOT(reg()));
}

void CReg::reg()
{
    QString strUsername = _username->text();
    QString strPassword = _password->text();
    QString strPassword2 = _password2->text();
    if(strUsername.length() == 0 || strPassword.length() == 0)
        return;
    if(strPassword != strPassword2)
        return;
#if 0
    int fd = xs_sock_connect(__xs_qq_port, __xs_qq_host);
    // md5
   char buf[64];
    xs_model_send_block_arg(fd, 4, __xsc_userman, __xsc_reg,
                            strUsername.toUtf8().data(),
                    xs_mkmd5_for_string(strPassword.toUtf8().data(), buf));

    // get response
    xs_model_t* model;
    xs_model_recv_block(&model, fd, 60000);
    ::close(fd);
#endif
    char buf[64];
    xs_model_t* model = qq_rpc_call(4, __xsc_userman, __xsc_reg,
                                    strUsername.toUtf8().data(),
                            xs_mkmd5_for_string(strPassword.toUtf8().data(), buf));

    if(model == NULL)
    {
        QMessageBox::warning(this, "Error", "model is NULL");
        return;
    }

    if(xs_success(model->argv(0)))
    {
        QMessageBox::information(this, "Success", "Reg success");
        accept();
    }
    else
    {
        QMessageBox::warning(this, "Error", QString("Reg failure:") + model->argv(1));
    }
    xs_model_delete(model);
}
