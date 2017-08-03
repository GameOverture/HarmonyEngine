#ifndef ENTITYWIDGET_H
#define ENTITYWIDGET_H

#include <QWidget>

namespace Ui {
class EntityWidget;
}

class EntityWidget : public QWidget
{
    Q_OBJECT

public:
    explicit EntityWidget(QWidget *parent = 0);
    ~EntityWidget();

private:
    Ui::EntityWidget *ui;
};

#endif // ENTITYWIDGET_H
