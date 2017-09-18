#include "PropertiesTreeView.h"
#include "PropertiesTreeItem.h"
#include "Global.h"
#include "WidgetVectorSpinBox.h"

#include <QPainter>
#include <QHeaderView>
#include <QCheckBox>
#include <QSpinBox>
#include <QDoubleSpinBox>

PropertiesTreeView::PropertiesTreeView(QWidget *pParent /*= nullptr*/) : QTreeView(pParent)
{
    setItemDelegate(new PropertiesDelegate(this, this));
}

PropertiesTreeView::~PropertiesTreeView()
{
}

/*virtual*/ void PropertiesTreeView::setModel(QAbstractItemModel *pModel) /*override*/
{
    QTreeView::setModel(pModel);

    //connect(
}

/*virtual*/ void PropertiesTreeView::paintEvent(QPaintEvent *pEvent) /*override*/
{
    QTreeView::paintEvent(pEvent);

    QPainter painter(viewport());
    for(int i = 0; i < header()->count(); ++i)
    {
        // draw only visible sections starting from second column
        if(header()->isSectionHidden(i) || header()->visualIndex(i) <= 0)
            continue;

        // position mapped to viewport
        int iColumnStartPos = header()->sectionViewportPosition(i) - 1;
        if(iColumnStartPos > 0)
        {
            //TODO: set QStyle::SH_Table_GridLineColor
            painter.drawLine(QPoint(iColumnStartPos, 0), QPoint(iColumnStartPos, height()));
        }
    }
}
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

PropertiesDelegate::PropertiesDelegate(PropertiesTreeView *pTableView, QObject *pParent /*= 0*/) :  QStyledItemDelegate(pParent),
                                                                                                    m_pTableView(pTableView)
{
}

/*virtual*/ QWidget *PropertiesDelegate::createEditor(QWidget *pParent, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    QWidget *pReturnWidget = nullptr;

    PropertiesTreeItem *pTreeItem = static_cast<PropertiesTreeItem *>(index.internalPointer());
    const PropertiesDef &defRef = pTreeItem->GetDataDef();

    switch(pTreeItem->GetType())
    {
    case PROPERTIESTYPE_bool:
//        pReturnWidget = new QCheckBox(pParent);

//        if(defRef.defaultValue.isValid())
//            static_cast<QCheckBox *>(pReturnWidget)->setCheckState(static_cast<Qt::CheckState>(defRef.defaultValue.toInt()));
        break;

    case PROPERTIESTYPE_int:
        pReturnWidget = new QSpinBox(pParent);

        if(defRef.defaultValue.isValid())
            static_cast<QSpinBox *>(pReturnWidget)->setValue(defRef.defaultValue.toInt());
        if(defRef.minRange.isValid() && defRef.maxRange.isValid())
            static_cast<QSpinBox *>(pReturnWidget)->setRange(defRef.minRange.toInt(), defRef.maxRange.toInt());
        if(defRef.stepAmt.isValid())
            static_cast<QSpinBox *>(pReturnWidget)->setSingleStep(defRef.stepAmt.toInt());
        if(defRef.sPrefix.isEmpty() == false)
            static_cast<QSpinBox *>(pReturnWidget)->setPrefix(defRef.sPrefix);
        if(defRef.sSuffix.isEmpty() == false)
            static_cast<QSpinBox *>(pReturnWidget)->setSuffix(defRef.sSuffix);
        break;

    case PROPERTIESTYPE_double:
        pReturnWidget = new QDoubleSpinBox(pParent);

        //static_cast<QDoubleSpinBox *>(pReturnWidget)->setDecimals(3);
        if(defRef.defaultValue.isValid())
            static_cast<QDoubleSpinBox *>(pReturnWidget)->setValue(defRef.defaultValue.toDouble());
        if(defRef.minRange.isValid() && defRef.maxRange.isValid())
            static_cast<QDoubleSpinBox *>(pReturnWidget)->setRange(defRef.minRange.toDouble(), defRef.maxRange.toDouble());
        if(defRef.stepAmt.isValid())
            static_cast<QDoubleSpinBox *>(pReturnWidget)->setSingleStep(defRef.stepAmt.toDouble());
        if(defRef.sPrefix.isEmpty() == false)
            static_cast<QDoubleSpinBox *>(pReturnWidget)->setPrefix(defRef.sPrefix);
        if(defRef.sSuffix.isEmpty() == false)
            static_cast<QDoubleSpinBox *>(pReturnWidget)->setSuffix(defRef.sSuffix);
        break;

    case PROPERTIESTYPE_ivec2:
        pReturnWidget = new WidgetVectorSpinBox(pParent);
        static_cast<WidgetVectorSpinBox *>(pReturnWidget)->SetAsInt(true);
        break;

    case PROPERTIESTYPE_vec2:
        pReturnWidget = new WidgetVectorSpinBox(pParent);
        static_cast<WidgetVectorSpinBox *>(pReturnWidget)->SetAsInt(false);
        break;
    }

    return pReturnWidget;
}

/*virtual*/ void PropertiesDelegate::setEditorData(QWidget *pEditor, const QModelIndex &index) const
{
    PropertiesTreeItem *pTreeItem = static_cast<PropertiesTreeItem *>(index.internalPointer());

    switch(pTreeItem->GetType())
    {
    case PROPERTIESTYPE_bool:
        //static_cast<QCheckBox *>(pEditor)->setCheckState(static_cast<Qt::CheckState>(pTreeItem->GetData().toInt()));
        break;
    case PROPERTIESTYPE_int:
        static_cast<QSpinBox *>(pEditor)->setValue(pTreeItem->GetData().toInt());
        break;
    case PROPERTIESTYPE_double:
        static_cast<QDoubleSpinBox *>(pEditor)->setValue(pTreeItem->GetData().toDouble());
        break;
    case PROPERTIESTYPE_ivec2:
        break;
    case PROPERTIESTYPE_vec2:
        break;
    default:
        HyGuiLog("Unsupported Delegate type", LOGTYPE_Error);
    }

//    QString sCurValue = index.model()->data(index, Qt::EditRole).toString();

//    switch(index.column())
//    {
//    case SpriteFramesModel::COLUMN_OffsetX:
//        static_cast<QSpinBox *>(pEditor)->setValue(sCurValue.toInt());
//        break;

//    case SpriteFramesModel::COLUMN_OffsetY:
//        static_cast<QSpinBox *>(pEditor)->setValue(sCurValue.toInt());
//        break;

//    case SpriteFramesModel::COLUMN_Duration:
//        static_cast<QDoubleSpinBox *>(pEditor)->setValue(sCurValue.toDouble());
//        break;
//    }
}

/*virtual*/ void PropertiesDelegate::setModelData(QWidget *pEditor, QAbstractItemModel *pModel, const QModelIndex &index) const
{
    PropertiesTreeItem *pTreeItem = static_cast<PropertiesTreeItem *>(index.internalPointer());
    switch(pTreeItem->GetType())
    {
    case PROPERTIESTYPE_bool:
        break;
    case PROPERTIESTYPE_int:
        pModel->setData(index, static_cast<QSpinBox *>(pEditor)->value());
        break;
    case PROPERTIESTYPE_double:
        pModel->setData(index, static_cast<QDoubleSpinBox *>(pEditor)->value());
        break;
    case PROPERTIESTYPE_ivec2:
        break;
    case PROPERTIESTYPE_vec2:
        break;
    default:
        HyGuiLog("Unsupported Delegate type", LOGTYPE_Error);
    }
//    case SpriteFramesModel::COLUMN_OffsetX:
//    {
//        SpriteFramesModel *pSpriteModel = static_cast<SpriteFramesModel *>(pModel);
//        QPoint vOffset = pSpriteModel->GetFrameAt(index.row())->m_vOffset;
//        vOffset.setX(static_cast<QSpinBox *>(pEditor)->value());

//        m_pItem->GetUndoStack()->push(new SpriteUndoCmd_OffsetFrame(m_pTableView, index.row(), vOffset));
//        break;
//    }

//    case SpriteFramesModel::COLUMN_OffsetY:
//    {
//        SpriteFramesModel *pSpriteModel = static_cast<SpriteFramesModel *>(pModel);
//        QPoint vOffset = pSpriteModel->GetFrameAt(index.row())->m_vOffset;
//        vOffset.setY(static_cast<QSpinBox *>(pEditor)->value());

//        m_pItem->GetUndoStack()->push(new SpriteUndoCmd_OffsetFrame(m_pTableView, index.row(), vOffset));
//        break;
//    }


//    case SpriteFramesModel::COLUMN_Duration:
//        m_pItem->GetUndoStack()->push(new SpriteUndoCmd_DurationFrame(m_pTableView, index.row(), static_cast<QDoubleSpinBox *>(pEditor)->value()));
//        break;
}

/*virtual*/ void PropertiesDelegate::updateEditorGeometry(QWidget *pEditor, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    pEditor->setGeometry(option.rect);
}

