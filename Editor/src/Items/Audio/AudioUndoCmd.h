/**************************************************************************
 *	AudioUndoCmd.h
 *
 *	Harmony Engine - Editor Tool
 *	Copyright (c) 2021 Jason Knobler
 *
 *	Harmony Editor Tool License:
 *	https://github.com/GameOverture/HarmonyEngine/blob/master/LICENSE
 *************************************************************************/
#ifndef AUDIOUNDOCMD_H
#define AUDIOUNDOCMD_H

#include <QUndoCommand>

class AudioAsset;
class ProjectItemData;
class ExplorerItemData;

enum AudioCmd
{
	AUDIOCMD_OrderAudio = 0,
	AUDIOCMD_RemoveWavs,
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
class AudioUndoCmd_AddAssets : public QUndoCommand
{
	ProjectItemData &				m_AudioItemRef;
	int								m_iStateIndex;

	QList<AudioAsset *>				m_AudioList;

public:
	AudioUndoCmd_AddAssets(ProjectItemData &audioItemRef, int iStateIndex, QList<AudioAsset *> audioList, QUndoCommand *pParent = nullptr);
	virtual ~AudioUndoCmd_AddAssets();

	virtual void redo() override;
	virtual void undo() override;
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
class AudioUndoCmd_RemoveAssets : public QUndoCommand
{
	ProjectItemData &			m_AudioItemRef;
	int							m_iStateIndex;

	QList<AudioAsset *>			m_AudioList;

public:
	AudioUndoCmd_RemoveAssets(ProjectItemData &audioItemRef, int iStateIndex, QList<AudioAsset *> audioList, QUndoCommand *pParent = nullptr);
	virtual ~AudioUndoCmd_RemoveAssets();

	virtual void redo() override;
	virtual void undo() override;
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

class AudioUndoCmd : public QUndoCommand
{
	const AudioCmd		m_eCMD;
	ProjectItemData &	m_AudioItemRef;
	QList<QVariant>		m_ParameterList;
	int					m_iStateIndex;

public:
	AudioUndoCmd(AudioCmd eCMD, ProjectItemData &audioItemRef, QList<QVariant> parameterList, QUndoCommand *pParent = nullptr);
	virtual ~AudioUndoCmd();

	virtual void redo() override;
	virtual void undo() override;
};

#endif // AUDIOUNDOCMD_H
