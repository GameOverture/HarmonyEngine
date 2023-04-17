/**************************************************************************
 *	AudioUndoCmd.cpp
 *
 *	Harmony Engine - Editor Tool
 *	Copyright (c) 2021 Jason Knobler
 *
 *	Harmony Editor Tool License:
 *	https://github.com/GameOverture/HarmonyEngine/blob/master/LICENSE
 *************************************************************************/
#include "Global.h"
#include "AudioUndoCmd.h"
#include "AudioModel.h"
#include "AudioWidget.h"

AudioUndoCmd_AddAssets::AudioUndoCmd_AddAssets(ProjectItemData &audioItemRef, int iStateIndex, QList<AudioAsset *> audioList, QUndoCommand *pParent /*= nullptr*/) :
	QUndoCommand(pParent),
	m_AudioItemRef(audioItemRef),
	m_iStateIndex(iStateIndex),
	m_AudioList(audioList)
{
	setText("Add " % QString::number(m_AudioList.size()) % " Audio Samples to State " % QString::number(iStateIndex));
}

/*virtual*/ AudioUndoCmd_AddAssets::~AudioUndoCmd_AddAssets()
{
}

/*virtual*/ void AudioUndoCmd_AddAssets::redo() /*override*/
{
	QVariant focusSubState = static_cast<AudioModel *>(m_AudioItemRef.GetModel())->Cmd_AddAudioAssets(m_iStateIndex, m_AudioList);
	m_AudioItemRef.FocusWidgetState(m_iStateIndex, focusSubState);
}

/*virtual*/ void AudioUndoCmd_AddAssets::undo() /*override*/
{
	static_cast<AudioModel *>(m_AudioItemRef.GetModel())->Cmd_RemoveAudioAssets(m_iStateIndex, m_AudioList);
	m_AudioItemRef.FocusWidgetState(m_iStateIndex, -1);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

AudioUndoCmd_RemoveAssets::AudioUndoCmd_RemoveAssets(ProjectItemData &audioItemRef, int iStateIndex, QList<AudioAsset *> audioList, QUndoCommand *pParent /*= nullptr*/) :
	QUndoCommand(pParent),
	m_AudioItemRef(audioItemRef),
	m_iStateIndex(iStateIndex),
	m_AudioList(audioList)
{
}

/*virtual*/ AudioUndoCmd_RemoveAssets::~AudioUndoCmd_RemoveAssets()
{
}

/*virtual*/ void AudioUndoCmd_RemoveAssets::redo() /*override*/
{
	static_cast<AudioModel *>(m_AudioItemRef.GetModel())->Cmd_RemoveAudioAssets(m_iStateIndex, m_AudioList);
	m_AudioItemRef.FocusWidgetState(m_iStateIndex, -1);
}

/*virtual*/ void AudioUndoCmd_RemoveAssets::undo() /*override*/
{
	QVariant focusSubState = static_cast<AudioModel *>(m_AudioItemRef.GetModel())->Cmd_AddAudioAssets(m_iStateIndex, m_AudioList);
	m_AudioItemRef.FocusWidgetState(m_iStateIndex, focusSubState);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

AudioUndoCmd::AudioUndoCmd(AudioCmd eCMD, ProjectItemData &audioItemRef, QList<QVariant> parameterList, QUndoCommand *pParent /*= nullptr*/) :
	QUndoCommand(pParent),
	m_eCMD(eCMD),
	m_ParameterList(parameterList),
	m_AudioItemRef(audioItemRef),
	m_iStateIndex(-1)
{
	if(m_AudioItemRef.GetType() != ITEM_Audio)
		HyGuiLog("AudioUndoCmds recieved wrong type: " % QString::number(m_AudioItemRef.GetType()) , LOGTYPE_Error);

	switch(m_eCMD)
	{
	case AUDIOCMD_OrderAudio:
		if(m_ParameterList.size() != 2)
			HyGuiLog("AudioUndoCmds - OrderAudio recieved incorrect m_ParameterList", LOGTYPE_Error);

		if(m_ParameterList[0].toInt() > m_ParameterList[1].toInt())
			setText("Order Audio Upwards");
		else
			setText("Order Audio Downwards");
		break;

	case AUDIOCMD_RemoveWavs:
		setText("Remove selected audio");
		break;
	}

	if(m_AudioItemRef.GetWidget())
		m_iStateIndex = m_AudioItemRef.GetWidget()->GetCurStateIndex();
}

/*virtual*/ AudioUndoCmd::~AudioUndoCmd()
{
}

/*virtual*/ void AudioUndoCmd::redo() /*override*/
{
	switch(m_eCMD)
	{
	case AUDIOCMD_OrderAudio: {
		AudioPlayListModel &playListModelRef = static_cast<AudioModel *>(m_AudioItemRef.GetModel())->GetPlayListModel(m_iStateIndex);
	
		int iOffset = m_ParameterList[1].toInt() - m_ParameterList[0].toInt();
		while(iOffset > 0)
		{
			playListModelRef.MoveRowDown(m_ParameterList[0].toInt());
			iOffset--;
		}
	
		while(iOffset < 0)
		{
			playListModelRef.MoveRowUp(m_ParameterList[0].toInt());
			iOffset++;
		}

		m_AudioItemRef.FocusWidgetState(m_iStateIndex, m_ParameterList[1].toInt());
		break; }

	case AUDIOCMD_RemoveWavs:
		break;
	}

	
}

/*virtual*/ void AudioUndoCmd::undo() /*override*/
{
	switch(m_eCMD)
	{
	case AUDIOCMD_OrderAudio: {
		//for(auto param : m_ParameterList)
		//{
		//	if(static_cast<SpineModel *>(m_SpineItemRef.GetModel())->GetNodeTreeModel().IsItemValid(param.value<TreeModelItemData *>(), true))
		//		static_cast<SpineModel *>(m_SpineItemRef.GetModel())->RemoveChild(param.value<TreeModelItemData *>());
		//}
		break; }

	case AUDIOCMD_RemoveWavs:
		break;
	}

	m_AudioItemRef.FocusWidgetState(m_iStateIndex, -1);
}
