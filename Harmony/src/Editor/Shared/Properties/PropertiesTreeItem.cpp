/**************************************************************************
*	PropertiesTreeItem.cpp
*
*	Harmony Engine - Editor Tool
*	Copyright (c) 2018 Jason Knobler
*
*	Harmony Editor Tool License:
*	https://github.com/GameOverture/HarmonyEngine/blob/master/LICENSE
*************************************************************************/
#include "Global.h"
#include "PropertiesTreeItem.h"
#include "PropertiesTreeModel.h"
#include "IModel.h"

PropertiesTreeItem::PropertiesTreeItem() :
	m_bREAD_ONLY(false)
{
}

PropertiesTreeItem::PropertiesTreeItem(QString sName, const PropertiesDef &propertiesDef, QColor color, QString sToolTip, bool bReadOnly) :

{
}

/*virtual*/ PropertiesTreeItem::~PropertiesTreeItem()
{
}

bool PropertiesTreeItem::IsCategory() const
{
	return m_DataDef.eType == PROPERTIESTYPE_Category || m_DataDef.eType == PROPERTIESTYPE_CategoryChecked;
}

bool PropertiesTreeItem::IsReadOnly() const
{
	return m_bREAD_ONLY;
}

PropertiesType PropertiesTreeItem::GetType() const
{
	return m_DataDef.eType;
}

QString PropertiesTreeItem::GetName() const
{
	return m_sNAME;
}



QVariant PropertiesTreeItem::GetData() const
{
	return m_Data;
}

const PropertiesDef &PropertiesTreeItem::GetDataDef() const
{
	return m_DataDef;
}

QColor PropertiesTreeItem::GetColor() const
{
	return m_Color;
}

void PropertiesTreeItem::SetData(const QVariant &newData)
{
	m_Data = newData;
}

/*virtual*/ QString PropertiesTreeItem::GetToolTip() const /*override*/
{
	return m_sToolTip;
}
