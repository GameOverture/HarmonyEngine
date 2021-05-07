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

enum SpineCmd
{
	SPINECMD_AddNewChildren = 0,
	SPINECMD_AddPrimitive,
};

class SpineUndoCmds : public QUndoCommand
{
	const SpineCmd		m_eCMD;
	ProjectItemData &	m_SpineItemRef;
	QList<QVariant>		m_ParameterList;
	int					m_iStateIndex;

public:
	SpineUndoCmds(SpineCmd eCMD, ProjectItemData &spineItemRef, QList<QVariant> parameterList, QUndoCommand *pParent = nullptr);
	virtual ~SpineUndoCmds();

	virtual void redo() override;
	virtual void undo() override;
};

#endif // SPINEUNDOCMDS_H
