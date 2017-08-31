#include "EntityWidget.h"
#include "ui_EntityWidget.h"

#include "EntityUndoCmds.h"

EntityWidget::EntityWidget(ProjectItem &itemRef, QWidget *parent) : QWidget(parent),
                                                                    ui(new Ui::EntityWidget),
                                                                    m_ItemRef(itemRef)
{
    ui->setupUi(this);

    ui->btnAddState->setDefaultAction(ui->actionAddState);
    ui->btnRemoveState->setDefaultAction(ui->actionRemoveState);
    ui->btnRenameState->setDefaultAction(ui->actionRenameState);
    ui->btnOrderStateBack->setDefaultAction(ui->actionOrderStateBackwards);
    ui->btnOrderStateForward->setDefaultAction(ui->actionOrderStateForwards);

    ui->btnAddChild->setDefaultAction(ui->actionAddSelectedChild);
    ui->btnAddChildPrimitive->setDefaultAction(ui->actionAddPrimitive);
    ui->btnInsertBoundingVolume->setDefaultAction(ui->actionInsertBoundingVolume);
    ui->btnInsertPhysics->setDefaultAction(ui->actionInsertPhysicsBody);

    ui->childrenTree->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    ui->childrenTree->setDragEnabled(true);
    ui->childrenTree->setDropIndicatorShown(true);
    ui->childrenTree->setDragDropMode(QAbstractItemView::InternalMove);

    EntityModel *pEntityModel = static_cast<EntityModel *>(m_ItemRef.GetModel());
    ui->childrenTree->setModel(&pEntityModel->GetTreeModel());
}

EntityWidget::~EntityWidget()
{
    delete ui;
}

ProjectItem &EntityWidget::GetItem()
{
    return m_ItemRef;
}

void EntityWidget::OnGiveMenuActions(QMenu *pMenu)
{
//    pMenu->addAction(ui->actionAddState);
//    pMenu->addAction(ui->actionRemoveState);
//    pMenu->addAction(ui->actionRenameState);
//    pMenu->addAction(ui->actionOrderStateBackwards);
//    pMenu->addAction(ui->actionOrderStateForwards);
//    pMenu->addSeparator();
//    pMenu->addAction(ui->actionAddLayer);
//    pMenu->addAction(ui->actionRemoveLayer);
//    pMenu->addAction(ui->actionOrderLayerUpwards);
//    pMenu->addAction(ui->actionOrderLayerDownwards);
}

void EntityWidget::on_actionAddSelectedChild_triggered()
{
    QUndoCommand *pCmd = new EntityUndoCmd_AddNewChild();
    m_ItemRef.GetUndoStack()->push(pCmd);
}

void EntityWidget::on_actionAddPrimitive_triggered()
{

}

void EntityWidget::on_actionInsertBoundingVolume_triggered()
{

}

void EntityWidget::on_actionInsertPhysicsBody_triggered()
{

}
