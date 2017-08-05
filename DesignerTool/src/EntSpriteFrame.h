#ifndef ENTSPRITEFRAME_H
#define ENTSPRITEFRAME_H

#include <QFrame>

namespace Ui {
class EntSpriteFrame;
}

class EntSpriteFrame : public QFrame
{
    Q_OBJECT

public:
    explicit EntSpriteFrame(QWidget *parent = 0);
    ~EntSpriteFrame();

private:
    Ui::EntSpriteFrame *ui;
};

#endif // ENTSPRITEFRAME_H
