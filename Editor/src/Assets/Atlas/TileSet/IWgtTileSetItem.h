/**************************************************************************
 *	IWgtTileSetItem.h
 *
 *	Harmony Engine - Editor Tool
 *	Copyright (c) 2025 Jason Knobler
 *
 *	Harmony Editor Tool License:
 *	https://github.com/GameOverture/HarmonyEngine/blob/master/LICENSE
 *************************************************************************/
#ifndef IWgtTileSetItem_H
#define IWgtTileSetItem_H

#include "Global.h"
#include "Project.h"

#include <QWidget>

class AuxTileSet;
class TileSetUndoCmd_ModifyWgtItem;

class IWgtTileSetItem : public QWidget
{
	Q_OBJECT
	
protected:
	const TileSetWgtType	m_eWGT_TYPE;
	QJsonObject				m_SerializedJsonObj;
	QUuid					m_Uuid;

	bool					m_bIsInitializing; // Prevents Modify UndoCmds from being created

	AuxTileSet *			m_pAuxTileSet;
	bool 					m_bIsSelected;

public:
	IWgtTileSetItem(TileSetWgtType eWgtType, QJsonObject initObj, AuxTileSet *pAuxTileSet, QWidget *pParent = nullptr);
	virtual ~IWgtTileSetItem();

	void Init(QJsonObject serializedObj);
	virtual void OnInit(QJsonObject serializedObj) = 0;

	TileSetWgtType GetWgtType() const;
	QUuid GetUuid() const;

	bool IsSelected() const;
	void SetSelected(bool bSelected);

	virtual QJsonObject SerializeCurrentWidgets() = 0;

protected:
	virtual QFrame* GetBorderFrame() const = 0;

	virtual bool eventFilter(QObject *watched, QEvent *event) override;

	void OnModifyWidget(QString sUndoText, int iMergeId);
};

#endif // IWgtTileSetItem_H
