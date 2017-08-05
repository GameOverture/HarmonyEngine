#ifndef ENTITYWIDGET_H
#define ENTITYWIDGET_H

#include "EntityModel.h"

#include <QWidget>

namespace Ui {
class EntityWidget;
}

class EntityWidget : public QWidget
{
    Q_OBJECT

    ProjectItem &               m_ItemRef;

public:
    explicit EntityWidget(ProjectItem &itemRef, QWidget *parent = 0);
    ~EntityWidget();

    ProjectItem &GetItem();

    void OnGiveMenuActions(QMenu *pMenu);

private:
    Ui::EntityWidget *ui;
};

#endif // ENTITYWIDGET_H
