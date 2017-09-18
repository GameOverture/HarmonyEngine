#include "PropertiesTreeItem.h"
#include "PropertiesTreeModel.h"

PropertiesTreeItem::PropertiesTreeItem(QString sName, PropertiesTreeModel *pTreeModel, const PropertiesDef &propertiesDef, QColor color) :  m_sNAME(sName),
                                                                                                                                            m_pTreeModel(pTreeModel),
                                                                                                                                            m_DataDef(propertiesDef),
                                                                                                                                            m_Color(color)
{
}

/*virtual*/ PropertiesTreeItem::~PropertiesTreeItem()
{
}

PropertiesType PropertiesTreeItem::GetType()
{
    return m_DataDef.eType;
}

QString PropertiesTreeItem::GetName()
{
    return m_sNAME;
}

QString PropertiesTreeItem::GetValue()
{
    QString sRetStr = m_DataDef.sPrefix;
    switch(m_DataDef.eType)
    {
    case PROPERTIESTYPE_Root:
    case PROPERTIESTYPE_Category:
    case PROPERTIESTYPE_bool:
        return QString();

    case PROPERTIESTYPE_int:
        sRetStr += QString::number(m_Data.toInt());
        break;
    case PROPERTIESTYPE_double:
        sRetStr += QString::number(m_Data.toDouble());
        break;
    case PROPERTIESTYPE_ivec2: {
            QPoint pt = m_Data.toPoint();
            sRetStr += QString::number(pt.x()) % " x " % QString::number(pt.y());
        }
        break;
    case PROPERTIESTYPE_vec2: {
            QPointF pt = m_Data.toPointF();
            sRetStr += QString::number(pt.x()) % " x " % QString::number(pt.y());
        }
        break;
    }

    sRetStr += m_DataDef.sSuffix;

    return sRetStr;
}

QVariant PropertiesTreeItem::GetData()
{
    return m_Data;
}

void PropertiesTreeItem::SetData(const QVariant &newData)
{
    m_Data = newData;
}

const PropertiesDef &PropertiesTreeItem::GetDataDef()
{
    return m_DataDef;
}

QColor PropertiesTreeItem::GetColor()
{
    return m_Color;
}

/*virtual*/ QString PropertiesTreeItem::GetToolTip() const /*override*/
{
    return QString();
}
