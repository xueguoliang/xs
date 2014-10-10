#include "clogin.h"

CLogin::CLogin(QWidget *parent) :
    QDialog(parent)
{
    QGridLayout* input = new QGridLayout;
    input->addWidget(new QLabel("Username:"), 0, 0);
    input->addWidget(_username = new QLineEdit, 0, 1);
    input->addWidget(new QLabel("Password:"), 1, 0);
    input->addWidget(_password = new QLineEdit, 1, 1);

    QHBoxLayout* hbox = new QHBoxLayout;
    QPushButton* button;
    hbox->addStretch(1);
    hbox->addWidget(button = new QPushButton("Login"));

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

    connect(button, SIGNAL(clicked()), this, SLOT(login()));

}


void CLogin::login()
{
    QString strUsername = _username->text();
    QString strPassword = _password->text();
    if(strUsername.length() == 0 || strPassword.length() == 0)
        return;
#if 0
    int fd = xs_sock_connect(__xs_qq_port, __xs_qq_host);
    // md5
   char buf[64];
    xs_model_send_block_arg(fd, 4, __xsc_userman, __xsc_login,
                            strUsername.toUtf8().data(),
                    xs_mkmd5_for_string(strPassword.toUtf8().data(), buf));

    // get response
    xs_model_t* model;
    xs_model_recv_block(&model, fd, 60000);
    ::close(fd);
#endif
    char buf[64];
    xs_model_t* model = qq_rpc_call(4, __xsc_userman, __xsc_login,
                                    strUsername.toUtf8().data(),
                            xs_mkmd5_for_string(strPassword.toUtf8().data(), buf));
    if(model == NULL)
    {
        QMessageBox::warning(this, "Error", "model is NULL");
        return;
    }
    if(xs_success(model->argv(0)))
    {
        QMessageBox::information(this, "Success", "Login success");
        accept();
    }
    else
    {
        QMessageBox::warning(this, "Error", QString("Login failure:") + model->argv(1));
    }
    xs_model_delete(model);


}

