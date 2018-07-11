/**************************************************************************
*	PropertiesTreeItem.cpp
*
*	Harmony Engine - Editor Tool
*	Copyright (c) 2018 Jason Knobler
*
*	Harmony Editor Tool License:
*	https://github.com/GameOverture/HarmonyEngine/blob/master/LICENSE
*************************************************************************/
#include "PropertiesTreeItem.h"
#include "PropertiesTreeModel.h"
#include "IModel.h"

PropertiesTreeItem::PropertiesTreeItem(QString sName, PropertiesTreeModel *pTreeModel, const PropertiesDef &propertiesDef, QColor color, QString sToolTip, bool bReadOnly) :    m_sNAME(sName),
																																												m_bREAD_ONLY(bReadOnly),
																																												m_pTreeModel(pTreeModel),
																																												m_DataDef(propertiesDef),
																																												m_Color(color),
																																												m_sToolTip(sToolTip)
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

QString PropertiesTreeItem::GetValue() const
{
	QString sRetStr = m_DataDef.sPrefix;
	switch(m_DataDef.eType)
	{
	case PROPERTIESTYPE_int:
	case PROPERTIESTYPE_Slider:
	case PROPERTIESTYPE_SpriteFrames:
		sRetStr += QString::number(m_Data.toInt());
		break;
	case PROPERTIESTYPE_double:
		sRetStr += QString::number(m_Data.toDouble());
		break;
	case PROPERTIESTYPE_ivec2: {
		QPoint pt = m_Data.toPoint();
		sRetStr += QString::number(pt.x()) % " x " % QString::number(pt.y());
		} break;
	case PROPERTIESTYPE_vec2: {
		QPointF pt = m_Data.toPointF();
		sRetStr += QString::number(pt.x()) % " x " % QString::number(pt.y());
		} break;
	case PROPERTIESTYPE_LineEdit:
		sRetStr += m_Data.toString();
		break;
	case PROPERTIESTYPE_ComboBox:
		sRetStr += m_DataDef.delegateBuilder.toStringList()[m_Data.toInt()];
		break;
	case PROPERTIESTYPE_StatesComboBox: {
		QComboBox tmpComboBox(nullptr);
		tmpComboBox.setModel(m_DataDef.delegateBuilder.value<ProjectItem *>()->GetModel());
		sRetStr += tmpComboBox.itemText(m_Data.toInt());
		} break;
	}

	sRetStr += m_DataDef.sSuffix;

	return sRetStr;
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
