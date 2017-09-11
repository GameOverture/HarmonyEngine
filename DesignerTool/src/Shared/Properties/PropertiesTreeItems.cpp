#include "PropertiesTreeItems.h"
#include "PropertiesModel.h"

PropertiesTreeItem::PropertiesTreeItem(PropertiesType eType, QString sName, PropertiesModel *pTreeModel) :  m_eTYPE(eType),
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
        return QString();

    case PROPERTIESTYPE_bool:

    case PROPERTIESTYPE_int:
    case PROPERTIESTYPE_double:
    case PROPERTIESTYPE_ivec2:
    case PROPERTIESTYPE_vec2:
        return QString();
    }

    return QString();
}

/*virtual*/ QString PropertiesTreeItem::GetToolTip() const /*override*/
{
    return QString();
}
