/**************************************************************************
 *	SpriteTableView.cpp
 *
 *	Harmony Engine - Designer Tool
 *	Copyright (c) 2016 Jason Knobler
 *
 *	The zlib License (zlib)
 *	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
 *************************************************************************/
#include "SpriteTableView.h"
#include "SpriteModels.h"
#include "Global.h"
#include "SpriteUndoCmds.h"

#include <QLineEdit>
#include <QDoubleSpinBox>

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

SpriteTableView::SpriteTableView(QWidget *pParent /*= 0*/) : QTableView(pParent)
{
}

/*virtual*/ void SpriteTableView::resizeEvent(QResizeEvent *pResizeEvent)
{
    int iWidth = pResizeEvent->size().width();

    iWidth -= 64 + 64 + 64;
    setColumnWidth(SpriteFramesModel::COLUMN_Frame, iWidth);
    setColumnWidth(SpriteFramesModel::COLUMN_OffsetX, 64);
    setColumnWidth(SpriteFramesModel::COLUMN_OffsetY, 64);
    setColumnWidth(SpriteFramesModel::COLUMN_Duration, 64);

    QTableView::resizeEvent(pResizeEvent);
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

WidgetSpriteDelegate::WidgetSpriteDelegate(ProjectItem *pItem, SpriteTableView *pTableView, QObject *pParent /*= 0*/) :  QStyledItemDelegate(pParent),
                                                                                                                         m_pItem(pItem),
                                                                                                                         m_pTableView(pTableView)
{
}

/*virtual*/ QWidget *WidgetSpriteDelegate::createEditor(QWidget *pParent, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    QWidget *pReturnWidget = NULL;

    switch(index.column())
    {
    case SpriteFramesModel::COLUMN_OffsetX:
        pReturnWidget = new QSpinBox(pParent);
        static_cast<QSpinBox *>(pReturnWidget)->setPrefix("X:");
        static_cast<QSpinBox *>(pReturnWidget)->setRange(-4096, 4096);
        break;
        
    case SpriteFramesModel::COLUMN_OffsetY:
        pReturnWidget = new QSpinBox(pParent);
        static_cast<QSpinBox *>(pReturnWidget)->setPrefix("Y:");
        static_cast<QSpinBox *>(pReturnWidget)->setRange(-4096, 4096);
        break;

    case SpriteFramesModel::COLUMN_Duration:
        pReturnWidget = new QDoubleSpinBox(pParent);
        static_cast<QDoubleSpinBox *>(pReturnWidget)->setSingleStep(0.001);
        static_cast<QDoubleSpinBox *>(pReturnWidget)->setDecimals(3);
        //static_cast<QDoubleSpinBox *>(pReturnWidget)->setSuffix("sec");
        break;
    }

    return pReturnWidget;
}

/*virtual*/ void WidgetSpriteDelegate::setEditorData(QWidget *pEditor, const QModelIndex &index) const
{
    QString sCurValue = index.model()->data(index, Qt::EditRole).toString();

    switch(index.column())
    {
    case SpriteFramesModel::COLUMN_OffsetX:
        static_cast<QSpinBox *>(pEditor)->setValue(sCurValue.toInt());
        break;
        
    case SpriteFramesModel::COLUMN_OffsetY:
        static_cast<QSpinBox *>(pEditor)->setValue(sCurValue.toInt());
        break;

    case SpriteFramesModel::COLUMN_Duration:
        static_cast<QDoubleSpinBox *>(pEditor)->setValue(sCurValue.toDouble());
        break;
    }
}

/*virtual*/ void WidgetSpriteDelegate::setModelData(QWidget *pEditor, QAbstractItemModel *pModel, const QModelIndex &index) const
{
    switch(index.column())
    {
    case SpriteFramesModel::COLUMN_OffsetX:
    {
        SpriteFramesModel *pSpriteModel = static_cast<SpriteFramesModel *>(pModel);
        QPoint vOffset = pSpriteModel->GetFrameAt(index.row())->m_vOffset;
        vOffset.setX(static_cast<QSpinBox *>(pEditor)->value());

        m_pItem->GetUndoStack()->push(new SpriteUndoCmd_OffsetFrame(m_pTableView, index.row(), vOffset, false));
        break;
    }

    case SpriteFramesModel::COLUMN_OffsetY:
    {
        SpriteFramesModel *pSpriteModel = static_cast<SpriteFramesModel *>(pModel);
        QPoint vOffset = pSpriteModel->GetFrameAt(index.row())->m_vOffset;
        vOffset.setY(static_cast<QSpinBox *>(pEditor)->value());

        m_pItem->GetUndoStack()->push(new SpriteUndoCmd_OffsetFrame(m_pTableView, index.row(), vOffset, false));
        break;
    }
    

    case SpriteFramesModel::COLUMN_Duration:
        m_pItem->GetUndoStack()->push(new SpriteUndoCmd_DurationFrame(m_pTableView, index.row(), static_cast<QDoubleSpinBox *>(pEditor)->value()));
        break;
    }
}

/*virtual*/ void WidgetSpriteDelegate::updateEditorGeometry(QWidget *pEditor, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    pEditor->setGeometry(option.rect);
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
