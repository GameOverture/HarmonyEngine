#include "EntityWidget.h"
#include "ui_EntityWidget.h"

EntityWidget::EntityWidget(ProjectItem &itemRef, QWidget *parent) : QWidget(parent),
                                                                    ui(new Ui::EntityWidget),
                                                                    m_ItemRef(itemRef)
{
    ui->setupUi(this);

    ui->childrenTree->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    ui->childrenTree->setDragEnabled(true);
    ui->childrenTree->setDropIndicatorShown(true);
    ui->childrenTree->setDragDropMode(QAbstractItemView::InternalMove);
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
