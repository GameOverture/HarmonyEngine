#include "PropertiesTreeItem.h"
#include "PropertiesTreeModel.h"

PropertiesTreeItem::PropertiesTreeItem(PropertiesType eType, QString sName, PropertiesTreeModel *pTreeModel) :  m_eTYPE(eType),
                                                                                                            m_sNAME(sName),
                                                                                                            m_pTreeModel(pTreeModel)
{
}

/*virtual*/ PropertiesTreeItem::~PropertiesTreeItem()
{
}

PropertiesType PropertiesTreeItem::GetType()
{
    return m_eTYPE;
}

QString PropertiesTreeItem::GetName()
{
    return m_sNAME;
}

QString PropertiesTreeItem::GetValue()
{
    switch(m_eTYPE)
    {
    case PROPERTIESTYPE_Root:
    case PROPERTIESTYPE_Category:
    case PROPERTIESTYPE_bool:
        return QString();

    case PROPERTIESTYPE_int:
        return QString::number(m_Data.toInt());

    case PROPERTIESTYPE_double:
        return QString::number(m_Data.toDouble());

    case PROPERTIESTYPE_ivec2: {
            QPoint pt = m_Data.toPoint();
            return QString::number(pt.x()) % " x " % QString::number(pt.y());
        }

    case PROPERTIESTYPE_vec2: {
            QPointF pt = m_Data.toPointF();
            return QString::number(pt.x()) % " x " % QString::number(pt.y());
        }
    }

    return QString();
}

QVariant PropertiesTreeItem::GetData()
{
    return m_Data;
}

void PropertiesTreeItem::SetData(const QVariant &newData)
{
    m_Data = newData;
}

QVariant PropertiesTreeItem::GetDataRanges()
{
    return m_DataRanges;
}

void PropertiesTreeItem::SetDataRanges(QVariant &ranges)
{
    m_DataRanges = ranges;
}

/*virtual*/ QString PropertiesTreeItem::GetToolTip() const /*override*/
{
    return QString();
}
