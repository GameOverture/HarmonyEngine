/**************************************************************************
*	PropertiesUndoCmd.h
*
*	Harmony Engine - Editor Tool
*	Copyright (c) 2018 Jason Knobler
*
*	Harmony Editor Tool License:
*	https://github.com/GameOverture/HarmonyEngine/blob/master/LICENSE
*************************************************************************/
#ifndef PROPERTIESUNDOCMD_H
#define PROPERTIESUNDOCMD_H

#include <QUndoCommand>
#include "PropertiesTreeMultiModel.h"

class PropertiesUndoCmd : public QUndoCommand
{
protected:
	ProjectItemData &		m_ItemRef;
	int						m_iStateIndex;
	QVariant				m_Substate;

	PropertiesTreeModel *	m_pModel;
	QPair<QString, QString> m_CatPropPair;
	bool 					m_bIsColumnNameToggle;

	QVariant				m_NewData;
	QVariant				m_OldData;

public:
	PropertiesUndoCmd(PropertiesTreeModel *pModel, const QModelIndex &index, const QVariant &newData, QUndoCommand *pParent = nullptr);
	virtual ~PropertiesUndoCmd();

	virtual void redo() override;
	virtual void undo() override;

protected:
	virtual void OnRedo();
	virtual void OnUndo();
};

#endif // PROPERTIESUNDOCMD_H
