#ifndef ENTITYCOMMON_H
#define ENTITYCOMMON_H

#include <QToolBox>

namespace Ui {
class EntityCommon;
}

class EntityCommon : public QToolBox
{
    Q_OBJECT

public:
    explicit EntityCommon(QWidget *parent = 0);
    ~EntityCommon();

private:
    Ui::EntityCommon *ui;
};

#endif // ENTITYCOMMON_H
