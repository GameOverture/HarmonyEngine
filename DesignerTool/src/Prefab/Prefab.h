#ifndef PREFAB_H
#define PREFAB_H

#include <QWidget>

namespace Ui {
class Prefab;
}

class Prefab : public QWidget
{
    Q_OBJECT

public:
    explicit Prefab(QWidget *parent = 0);
    ~Prefab();

private:
    Ui::Prefab *ui;
};

#endif // PREFAB_H
