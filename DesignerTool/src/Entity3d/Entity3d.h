#ifndef ENTITY3D_H
#define ENTITY3D_H

#include <QWidget>

namespace Ui {
class Entity3d;
}

class Entity3d : public QWidget
{
    Q_OBJECT

public:
    explicit Entity3d(QWidget *parent = 0);
    ~Entity3d();

private:
    Ui::Entity3d *ui;
};

#endif // ENTITY3D_H
