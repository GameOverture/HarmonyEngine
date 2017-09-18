#include "PropertiesTreeView.h"
#include "PropertiesTreeItem.h"
#include "Global.h"
#include "WidgetVectorSpinBox.h"

#include <QPainter>
#include <QHeaderView>
#include <QCheckBox>
#include <QSpinBox>
#include <QDoubleSpinBox>
#include <QLineEdit>

PropertiesTreeView::PropertiesTreeView(QWidget *pParent /*= nullptr*/) : QTreeView(pParent)
{
    setItemDelegate(new PropertiesDelegate(this, this));

    this->setIndentation(0);
    this->setAnimated(true);
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
        // Handled natively within tree model's CheckStateRole
        break;

    case PROPERTIESTYPE_int:
        pReturnWidget = new QSpinBox(pParent);

        if(defRef.defaultData.isValid())
            static_cast<QSpinBox *>(pReturnWidget)->setValue(defRef.defaultData.toInt());
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

        if(defRef.defaultData.isValid())
            static_cast<QDoubleSpinBox *>(pReturnWidget)->setValue(defRef.defaultData.toDouble());
        if(defRef.minRange.isValid() && defRef.maxRange.isValid())
            static_cast<QDoubleSpinBox *>(pReturnWidget)->setRange(defRef.minRange.toDouble(), defRef.maxRange.toDouble());
        if(defRef.stepAmt.isValid())
            static_cast<QDoubleSpinBox *>(pReturnWidget)->setSingleStep(defRef.stepAmt.toDouble());
        if(defRef.sPrefix.isEmpty() == false)
            static_cast<QDoubleSpinBox *>(pReturnWidget)->setPrefix(defRef.sPrefix);
        if(defRef.sSuffix.isEmpty() == false)
            static_cast<QDoubleSpinBox *>(pReturnWidget)->setSuffix(defRef.sSuffix);
        if(defRef.delegateBuilder.isValid())
            static_cast<QDoubleSpinBox *>(pReturnWidget)->setDecimals(defRef.delegateBuilder.toInt());
        break;

    case PROPERTIESTYPE_ivec2:
        pReturnWidget = new WidgetVectorSpinBox(true, pParent);

        if(defRef.defaultData.isValid())
            static_cast<WidgetVectorSpinBox *>(pReturnWidget)->SetValue(defRef.defaultData);
        break;

    case PROPERTIESTYPE_vec2:
        pReturnWidget = new WidgetVectorSpinBox(false, pParent);

        if(defRef.defaultData.isValid())
            static_cast<WidgetVectorSpinBox *>(pReturnWidget)->SetValue(defRef.defaultData);
        break;

    case PROPERTIESTYPE_LineEdit:
        pReturnWidget = new QLineEdit(pParent);

        if(defRef.defaultData.isValid())
            static_cast<QLineEdit *>(pReturnWidget)->setText(defRef.defaultData.toString());
        break;

    case PROPERTIESTYPE_ComboBox:
        pReturnWidget = new QComboBox(pParent);

        if(defRef.delegateBuilder.isValid())
            static_cast<QComboBox *>(pReturnWidget)->addItems(defRef.delegateBuilder.toStringList());
        if(defRef.defaultData.isValid())
            static_cast<QComboBox *>(pReturnWidget)->setCurrentIndex(defRef.defaultData.toInt());
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
        // Handled natively within tree model's CheckStateRole
        break;
    case PROPERTIESTYPE_int:
        static_cast<QSpinBox *>(pEditor)->setValue(pTreeItem->GetData().toInt());
        break;
    case PROPERTIESTYPE_double:
        static_cast<QDoubleSpinBox *>(pEditor)->setValue(pTreeItem->GetData().toDouble());
        break;
    case PROPERTIESTYPE_ivec2:
    case PROPERTIESTYPE_vec2:
        static_cast<WidgetVectorSpinBox *>(pEditor)->SetValue(pTreeItem->GetData());
        break;
    case PROPERTIESTYPE_LineEdit:
        static_cast<QLineEdit *>(pEditor)->setText(pTreeItem->GetData().toString());
        break;
    case PROPERTIESTYPE_ComboBox:
        static_cast<QComboBox *>(pEditor)->setCurrentIndex(pTreeItem->GetData().toInt());
        break;
    default:
        HyGuiLog("PropertiesDelegate::setEditorData() Unsupported Delegate type:" % QString::number(pTreeItem->GetType()), LOGTYPE_Error);
    }
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
    case PROPERTIESTYPE_vec2:
        pModel->setData(index, static_cast<WidgetVectorSpinBox *>(pEditor)->GetValue());
        break;
    case PROPERTIESTYPE_LineEdit:
        pModel->setData(index, static_cast<QLineEdit *>(pEditor)->text());
        break;
    case PROPERTIESTYPE_ComboBox:
        pModel->setData(index, static_cast<QComboBox *>(pEditor)->currentIndex());
        break;
    default:
        HyGuiLog("PropertiesDelegate::setModelData() Unsupported Delegate type:" % QString::number(pTreeItem->GetType()), LOGTYPE_Error);
    }
}

/*virtual*/ void PropertiesDelegate::updateEditorGeometry(QWidget *pEditor, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    pEditor->setGeometry(option.rect);
}

