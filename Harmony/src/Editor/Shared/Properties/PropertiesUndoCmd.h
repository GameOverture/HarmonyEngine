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
#include "PropertiesTreeModel.h"

class PropertiesUndoCmd : public QUndoCommand
{
	PropertiesTreeModel *	m_pModel;
	QModelIndex				m_ModelIndex;

	QVariant				m_NewData;
	QVariant				m_OldData;

public:
	PropertiesUndoCmd(PropertiesTreeModel *pModel, const QModelIndex &index, const QVariant &newData, QUndoCommand *pParent = nullptr);
	virtual ~PropertiesUndoCmd();

	virtual void redo() override;
	virtual void undo() override;
};

#endif // PROPERTIESUNDOCMD_H
