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

class ProjectItemData;
class ExplorerItemData;

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

#endif // SPINEUNDOCMDS_H
