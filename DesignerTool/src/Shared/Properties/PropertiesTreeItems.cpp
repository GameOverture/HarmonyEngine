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

/*virtual*/ QString PropertiesTreeItem::GetToolTip() const /*override*/
{
    return QString();
}
