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
    EntityStateData *GetCurStateData();
    int GetNumStates() const;

private Q_SLOTS:
    void on_actionAddSelectedChild_triggered();
    void on_actionAddPrimitive_triggered();
    void on_actionInsertBoundingVolume_triggered();
    void on_actionInsertPhysicsBody_triggered();

    void on_childrenTree_clicked(const QModelIndex &index);

    void on_actionRenameState_triggered();
    void on_actionAddState_triggered();
    void on_actionRemoveState_triggered();
    void on_actionOrderStateBackwards_triggered();
    void on_actionOrderStateForwards_triggered();

private:
    Ui::EntityWidget *ui;
};

#endif // ENTITYWIDGET_H
