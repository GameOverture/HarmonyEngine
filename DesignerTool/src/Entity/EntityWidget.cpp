#include "EntityWidget.h"
#include "ui_EntityWidget.h"
#include "Project.h"
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

EntityModel *EntityWidget::GetEntityModel()
{
    return static_cast<EntityModel *>(m_ItemRef.GetModel());
}

EntityTreeItem *EntityWidget::GetCurSelectedTreeItem()
{
    QModelIndex curIndex = ui->childrenTree->currentIndex();
    return static_cast<EntityTreeItem *>(curIndex.internalPointer());

    //return static_cast<EntityTreeItem *>(GetEntityModel()->GetTreeModel().index(curIndex.row(), curIndex.column(), curIndex.parent()).internalPointer());
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
    if(GetCurSelectedTreeItem() == nullptr)
    {
        HyGuiLog("Currently selected entity tree item is nullptr. Cannot add child.", LOGTYPE_Error);
        return;
    }

    DataExplorerItem *pExplorerItem = m_ItemRef.GetProject().GetExplorerWidget()->GetCurItemSelected();
    if(pExplorerItem->IsProjectItem() == false)
    {
        HyGuiLog("Currently selected item in Explorer is not a ProjectItem. Cannot add child to entity.", LOGTYPE_Error);
        return;
    }

    ProjectItem *pItem = static_cast<ProjectItem *>(pExplorerItem);
    QUndoCommand *pCmd = new EntityUndoCmd_AddNewChild(GetCurSelectedTreeItem(), &GetEntityModel()->GetTreeModel(), pItem);
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

void EntityWidget::on_childrenTree_clicked(const QModelIndex &index)
{
    EntityTreeItem *pTreeItem = static_cast<EntityTreeItem *>(index.internalPointer());
    if(pTreeItem == nullptr)
    {
        ui->stackedWidget->setCurrentIndex(STACKED_Null);
        return;
    }

    switch(pTreeItem->GetItem()->GetType())
    {
    case ITEM_Sprite:
        ui->stackedWidget->setCurrentIndex(STACKED_Sprite);
        break;
    case ITEM_Entity:
        ui->stackedWidget->setCurrentIndex(STACKED_Entity);
        break;
    case ITEM_Font:
        ui->stackedWidget->setCurrentIndex(STACKED_Font);
        break;
    case ITEM_AtlasImage:
        ui->stackedWidget->setCurrentIndex(STACKED_TexturedQuad);
        break;
    case ITEM_Audio:
    case ITEM_Particles:
    case ITEM_Spine:
    case ITEM_Shader:
    case ITEM_BoundingVolume:
        ui->stackedWidget->setCurrentIndex(STACKED_BoundingVolume);
        break;
    case ITEM_Physics:
        ui->stackedWidget->setCurrentIndex(STACKED_Physics);
        break;
        HyGuiLog("Unsupported Entity childrenTree clicked", LOGTYPE_Error);
        break;
    }

}
