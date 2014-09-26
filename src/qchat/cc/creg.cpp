#include "creg.h"

CReg::CReg(QWidget *parent) :
    QDialog(parent)
{
    QGridLayout* grid = new QGridLayout;
    grid->addWidget(new QLabel("用户名："), 0, 0);
    grid->addWidget(_username = new QLineEdit, 0, 1);
    grid->addWidget(new QLabel("密码："), 1, 0);
    grid->addWidget(_password = new QLineEdit, 1, 1);
    grid->addWidget(new QLabel("再输入一次密码："), 2, 0);
    grid->addWidget(_passwordAgain = new QLineEdit, 2, 1);

    QPushButton* buttonOK = new QPushButton("注册");
    QPushButton* buttonClose = new QPushButton("关闭");
    QHBoxLayout* hBox = new QHBoxLayout;
    hBox->addStretch(1);
    hBox->addWidget(buttonOK);
    hBox->addWidget(buttonClose);

    QVBoxLayout* vLayout = new QVBoxLayout;
    vLayout->addLayout(grid);
    vLayout->addLayout(hBox);

    QGridLayout* layout = new QGridLayout;
    layout->addLayout(vLayout, 1, 1);
    layout->setColumnStretch(0, 1);
    layout->setColumnStretch(2, 1);
    layout->setRowStretch(0, 1);
    layout->setRowStretch(2, 1);

    setLayout(layout);

    _password->setEchoMode(QLineEdit::Password);
    _passwordAgain->setEchoMode(QLineEdit::Password);

    connect(buttonOK, SIGNAL(clicked()), this, SLOT(doReg()));
    connect(buttonClose, SIGNAL(clicked()), this, SLOT(reject()));
}

/*
    model0 = userman
    model1 = reg
    model2 = username
    model3 = password
*/
void CReg::doReg()
{
    int ret;
    if(_username->text().length() == 0 || _password->text().length() == 0)
    {
        QMessageBox::warning(this, "Error", "Username or Password can not be NULL");
        return;
    }
    if(_password->text() != _passwordAgain->text())
    {
        QMessageBox::warning(this, "Error", "Password not same");
        return;
    }
    // connect to server
    int fd = xs_sock_connect(__XS_CHAT_PORT, "127.0.0.1");
    if(fd < 0)
    {
        QMessageBox::warning(this, "Error", "Connect to server Error");
        return;
    }
    qDebug() << "connect success";

    // make the packet to server
    xs_model_t* model = xs_model_create(4);
    model->argv[0] = xs_strdup(__xsc_userman);
    model->argv[1] = xs_strdup(__xsc_reg);
    model->argv[2] = xs_strdup(_username->text().toUtf8().data());
    // must transfer to MD5
    char md5[64];
    xs_mkmd5_for_string(_password->text().toUtf8().data(), md5);
    model->argv[3] = xs_strdup(md5);

    // send model to server
    ret = xs_model_send_block(model, fd, 60000);
    xs_model_delete(model);
    if(ret != 0)
    {
        QMessageBox::warning(this, "Error", "Send data error");
        ::close(fd);
        return;
    }

    // recv the data from server
    ret = xs_model_recv_block(&model, fd, 6000);
    if(ret != 0)
    {
        ::close(fd);
        QMessageBox::warning(this, "Error", "Recv reg data error");
        return;
    }

    if(xs_success(model->argv[0]))
    {
        ::close(fd);
        QMessageBox::information(this, "Info", "Register success");
        accept();
        xs_model_delete(model);
        return;
    }

    QMessageBox::warning(this, "Error", QString("Reg Error:")+model->argv(1));
    xs_model_delete(model);
    ::close(fd);
}
