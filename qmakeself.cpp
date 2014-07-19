#include "qmakeself.h"
#include "ui_qmakeself.h"

QMakeSelf::QMakeSelf(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::QMakeSelf)
{
    ui->setupUi(this);
}

QMakeSelf::~QMakeSelf()
{
    delete ui;
}

void QMakeSelf::changeEvent(QEvent *e)
{
    QMainWindow::changeEvent(e);
    switch (e->type()) {
    case QEvent::LanguageChange:
        ui->retranslateUi(this);
        break;
    default:
        break;
    }
}
