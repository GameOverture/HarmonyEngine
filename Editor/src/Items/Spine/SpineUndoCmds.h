/**************************************************************************
 *	SpineUndoCmds.h
 *
 *	Harmony Engine - Editor Tool
 *	Copyright (c) 2021 Jason Knobler
 *
 *	Harmony Editor Tool License:
 *	https://github.com/GameOverture/HarmonyEngine/blob/master/LICENSE
 *************************************************************************/
#ifndef SPINEUNDOCMDS_H
#define SPINEUNDOCMDS_H

#include <QUndoCommand>
#include <QTableView>

class ProjectItemData;

class SpineUndoCmd_AddNewCrossFade : public QUndoCommand
{
	ProjectItemData &				m_SpineItemRef;

	QString 						m_sAnimOne;
	QString 						m_sAnimTwo;
	float							m_fMixValue;

public:
	SpineUndoCmd_AddNewCrossFade(ProjectItemData &spineItemRef, QString sAnimOne, QString sAnimTwo, float fMixValue, QUndoCommand *pParent = nullptr);
	virtual ~SpineUndoCmd_AddNewCrossFade();

	virtual void redo() override;
	virtual void undo() override;
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

class SpineUndoCmd_ModifyCrossFade : public QUndoCommand
{
	ProjectItemData &				m_SpineItemRef;

	QTableView *					m_pTableView;
	QModelIndex						m_Index;
	QVariant						m_NewData;
	QVariant						m_OldData;

public:
	SpineUndoCmd_ModifyCrossFade(ProjectItemData &spineItemRef, QTableView *pTableView, QModelIndex index, QVariant newData, QUndoCommand *pParent = nullptr);
	virtual ~SpineUndoCmd_ModifyCrossFade();

	virtual void redo() override;
	virtual void undo() override;
};

#endif // SPINEUNDOCMDS_H
