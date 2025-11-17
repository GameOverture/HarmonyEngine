/**************************************************************************
 *	IWgtTileSetItem.cpp
 *
 *	Harmony Engine - Editor Tool
 *	Copyright (c) 2025 Jason Knobler
 *
 *	Harmony Editor Tool License:
 *	https://github.com/GameOverture/HarmonyEngine/blob/master/LICENSE
 *************************************************************************/
#include "Global.h"
#include "IWgtTileSetItem.h"
#include "AuxTileSet.h"
#include "TileSetUndoCmds.h"

#include <QMouseEvent>
#include <QPushButton>

IWgtTileSetItem::IWgtTileSetItem(TileSetWgtType eWgtType, QJsonObject initObj, AuxTileSet *pAuxTileSet, QWidget *pParent /*= nullptr*/) :
	QWidget(pParent),
	m_eWGT_TYPE(eWgtType),
	m_SerializedJsonObj(initObj),
	m_Uuid(QUuid(initObj["UUID"].toString())),
	m_bIsInitializing(false),
	m_pAuxTileSet(pAuxTileSet),
	m_bIsSelected(false)
{
}

IWgtTileSetItem::~IWgtTileSetItem()
{
}

void IWgtTileSetItem::Init(QJsonObject serializedObj)
{
	m_bIsInitializing = true;
	blockSignals(true);
	OnInit(serializedObj);
	blockSignals(false);
	m_bIsInitializing = false;
}

TileSetWgtType IWgtTileSetItem::GetWgtType() const
{
	return m_eWGT_TYPE;
}

QUuid IWgtTileSetItem::GetUuid() const
{
	return m_Uuid;
}

bool IWgtTileSetItem::IsSelected() const
{
	return m_bIsSelected;
}

void IWgtTileSetItem::SetSelected(bool bSelected)
{
	m_bIsSelected = bSelected;

	QFrame* pBorderFrame = GetBorderFrame();
	if (m_bIsSelected)
	{
		pBorderFrame->setFrameStyle(QFrame::Box | QFrame::Raised);
		pBorderFrame->setLineWidth(2);
		pBorderFrame->setEnabled(true);
	}
	else
	{
		pBorderFrame->setFrameStyle(QFrame::Box | QFrame::Plain);
		pBorderFrame->setLineWidth(1);
		pBorderFrame->setEnabled(false);
	}

	pBorderFrame->installEventFilter(this);
}

/*virtual*/ bool IWgtTileSetItem::eventFilter(QObject *watched, QEvent *event) /*override*/
{
	if (event->type() == QEvent::MouseButtonPress)
	{
		QMouseEvent* pMouseEvent = static_cast<QMouseEvent*>(event);
		if (pMouseEvent->button() == Qt::LeftButton || pMouseEvent->button() == Qt::RightButton)
		{
			if (IsSelected() == false)
			{
				m_pAuxTileSet->MakeSelectionChange(this);
				return true; // eat event
			}
		}
	}

	return QWidget::eventFilter(watched, event);
}

void IWgtTileSetItem::OnModifyWidget(QString sUndoText, int iMergeId)
{
	QJsonObject oldItemDataObj = m_SerializedJsonObj;
	QJsonObject newItemDataObj = SerializeCurrentWidgets();

	if (m_bIsInitializing)
		return;

	// Compare all the keys and ensure they are the same
	bool bDifferenceFound = false;
	QString sOldValue;
	QString sNewValue;
	for (QString sKey : oldItemDataObj.keys())
	{
		if (newItemDataObj.contains(sKey) == false)
			HyGuiLog("IWgtTileSetItem::OnModifyWidget: Key '" + sKey + "' missing from new item data object!", LOGTYPE_Error);
		else
		{
			QJsonValueRef oldValueRef = oldItemDataObj[sKey];
			QJsonValueRef newValueRef = newItemDataObj[sKey];
			if (oldValueRef.type() != newValueRef.type())
				HyGuiLog("IWgtTileSetItem::OnModifyWidget: Key '" + sKey + "' type mismatch!", LOGTYPE_Error);
			
			switch (oldItemDataObj[sKey].type())
			{
			case QJsonValue::Type::Bool:
				sOldValue = oldItemDataObj[sKey].toBool() ? "true" : "false";
				sNewValue = newItemDataObj[sKey].toBool() ? "true" : "false";
				bDifferenceFound = sOldValue != sNewValue;
				break;
			case QJsonValue::Type::Double:
				sOldValue = QString::number(oldItemDataObj[sKey].toDouble());
				sNewValue = QString::number(newItemDataObj[sKey].toDouble());
				bDifferenceFound = qAbs(oldItemDataObj[sKey].toDouble() - newItemDataObj[sKey].toDouble()) > 0.001; // Compare doubles with a tolerance
				break;
	
			case QJsonValue::Type::String:
				sOldValue = oldItemDataObj[sKey].toString();
				sNewValue = newItemDataObj[sKey].toString();
				bDifferenceFound = sOldValue != sNewValue;
				break;

			default:
				bDifferenceFound = true;
				break;
			}
		}
	}
	for (QString sKey : newItemDataObj.keys())
	{
		if (oldItemDataObj.contains(sKey) == false)
			HyGuiLog("IWgtTileSetItem::OnModifyWidget: Key '" + sKey + "' missing from old item data object!", LOGTYPE_Error);
	}
	if (bDifferenceFound == false)
		return; // No changes detected

	if(sOldValue.isEmpty() == false && sNewValue.isEmpty() == false)
		sUndoText += " (" + sOldValue + " -> " + sNewValue + ")";
	TileSetUndoCmd_ModifyWgtItem *pNewCmd = new TileSetUndoCmd_ModifyWgtItem(*m_pAuxTileSet, sUndoText, iMergeId, m_Uuid, oldItemDataObj, newItemDataObj);
	m_pAuxTileSet->GetTileSet()->GetUndoStack()->push(pNewCmd);
}

void IWgtTileSetItem::SetButtonColor(QPushButton *pBtn, HyColor color)
{
	pBtn->setFlat(true);
	pBtn->setStyleSheet("QPushButton { background-color: #" + QString::number(color.GetAsHexCode(), 16).rightJustified(6, '0') + "; border: 1px solid black; }"
						"QPushButton::hover { border: 2px solid black; }");
}
