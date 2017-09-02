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

    enum StackedWidgetLayer
    {
        STACKED_Null = 0,
        STACKED_Entity,
        STACKED_Primitive,
        STACKED_TexturedQuad,
        STACKED_Sprite,
        STACKED_Font,
        STACKED_BoundingVolume,
        STACKED_Physics,

        NUMSTACKED
    };

    ProjectItem &               m_ItemRef;

public:
    explicit EntityWidget(ProjectItem &itemRef, QWidget *parent = 0);
    ~EntityWidget();

    ProjectItem &GetItem();
    EntityModel *GetEntityModel();

    EntityTreeItem *GetCurSelectedTreeItem();

    void OnGiveMenuActions(QMenu *pMenu);

private Q_SLOTS:
    void on_actionAddSelectedChild_triggered();
    void on_actionAddPrimitive_triggered();
    void on_actionInsertBoundingVolume_triggered();
    void on_actionInsertPhysicsBody_triggered();


    void on_childrenTree_clicked(const QModelIndex &index);

private:
    Ui::EntityWidget *ui;
};

#endif // ENTITYWIDGET_H
