#ifndef QMAKESELF_H
#define QMAKESELF_H

#include <QMainWindow>
#include <QToolBar>

namespace Ui {
class QMakeSelf;
}

class QMakeSelf : public QMainWindow
{
    Q_OBJECT

public:
    explicit QMakeSelf(QWidget *parent = 0);
    ~QMakeSelf();

protected:
    void changeEvent(QEvent *e);

private:
    Ui::QMakeSelf *ui;
};

#endif // QMAKESELF_H
