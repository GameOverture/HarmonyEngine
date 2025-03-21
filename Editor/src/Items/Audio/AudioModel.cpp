/**************************************************************************
*	AudioModel.cpp
*
*	Harmony Engine - Editor Tool
*	Copyright (c) 2019 Jason Knobler
*
*	Harmony Editor Tool License:
*	https://github.com/GameOverture/HarmonyEngine/blob/master/LICENSE
*************************************************************************/
#include "Global.h"
#include "AudioModel.h"
#include "AudioManagerModel.h"
#include "Project.h"

AudioStateData::AudioStateData(int iStateIndex, IModel &modelRef, FileDataPair stateFileData) :
	IStateData(iStateIndex, modelRef, stateFileData),
	m_PlayListModel(&modelRef),
	m_PropertiesModel(modelRef.GetItem(), iStateIndex, -1, &modelRef)
{
	QJsonArray metaAssetArray = stateFileData.m_Meta["assetUUIDs"].toArray();
	QJsonArray dataPlayListArray = stateFileData.m_Data["playlist"].toArray();

	QList<QUuid> uuidRequestList;
	for(int i = 0; i < metaAssetArray.size(); ++i)
		uuidRequestList.append(QUuid(metaAssetArray[i].toString()));

	QList<TreeModelItemData *> dependeeList = m_ModelRef.GetItem().GetProject().IncrementDependencies(&m_ModelRef.GetItem(), uuidRequestList);
	for(int i = 0; i < dependeeList.size(); ++i)
		Cmd_AddAudioAsset(static_cast<SoundClip *>(dependeeList[i]));

	if(dataPlayListArray.size() != dependeeList.size())
		HyGuiLog("AudioStateData::AudioStateData() failed to acquire all the stored audio assets", LOGTYPE_Error);
	
	const int iRANGE = 16777215;	// Uses 3 bytes (0xFFFFFF)... Qt uses this value for their default ranges in QSpinBox

	m_PropertiesModel.AppendCategory("Play List");
	m_PropertiesModel.AppendProperty("Play List", "Mode", PROPERTIESTYPE_ComboBoxInt, stateFileData.m_Data["playlistMode"].toInt(0), "The method by which the next audio asset is chosen when this item is played", PROPERTIESACCESS_Mutable, QVariant(), QVariant(), QVariant(), QString(), QString(), HyGlobal::GetAudioPlayListModeList());

	m_PropertiesModel.AppendCategory("Mixing");
	m_PropertiesModel.AppendProperty("Mixing", "Volume", PROPERTIESTYPE_double, stateFileData.m_Data["volume"].toDouble(1.0), "Volume modifier [0.0 to 1.0] applied to source volume", PROPERTIESACCESS_Mutable, 0.0, 1.0, 0.1, QString(), QString(), 4);
	m_PropertiesModel.AppendProperty("Mixing", "Pitch", PROPERTIESTYPE_double, stateFileData.m_Data["pitch"].toDouble(1.0), "Pitch modifier [0.0 to 1.0] applied to source volume", PROPERTIESACCESS_Mutable, 0.0, 1.0, 0.1, QString(), QString(), 4);
	m_PropertiesModel.AppendProperty("Mixing", "Priority", PROPERTIESTYPE_int, stateFileData.m_Data["priority"].toInt(0), "Audio assets with higher priority will take presidence over others when channels are maxed out", PROPERTIESACCESS_Mutable, -iRANGE, iRANGE, 1);

	int iLoops = stateFileData.m_Data["loops"].toInt(0);
	m_PropertiesModel.AppendCategory("Looping", QVariant(), true, "The audio asset will play for loops+1 number of times, until stopped");
	m_PropertiesModel.AppendProperty("Looping", "Num Loops", PROPERTIESTYPE_int, iLoops != 0 ? iLoops : -1, "Number of loops, -1 is infinite loops. Passing one here plays the audio asset twice (1 loop)", PROPERTIESACCESS_Mutable, -1);

	int iDist = stateFileData.m_Data["maxDist"].toInt(0);
	m_PropertiesModel.AppendCategory("Positional", QVariant(), true, "Emulates a simple 3D audio effect based on the item's position relative to the listening camera");
	m_PropertiesModel.AppendProperty("Positional", "Max Distance", PROPERTIESTYPE_int, iDist != 0 ? iDist : 1, "The distance (and beyond) at which the audio asset will be played at its quietest", PROPERTIESACCESS_Mutable, 0, iRANGE, 1);

	if(iLoops != 0)
		m_PropertiesModel.SetToggleState("Looping", Qt::Checked);

	if(iDist != 0)
		m_PropertiesModel.SetToggleState("Positional", Qt::Checked);
}

/*virtual*/ AudioStateData::~AudioStateData()
{
}

AudioPlayListModel &AudioStateData::GetPlayListModel()
{
	return m_PlayListModel;
}

PropertiesTreeModel &AudioStateData::GetPropertiesModel()
{
	return m_PropertiesModel;
}

QList<TreeModelItemData *> AudioStateData::GetSoundClips() const
{
	return m_PlayListModel.GetSoundClips();
}

QJsonArray AudioStateData::GenPlayListArray() const
{
	return m_PlayListModel.GenPlayListArray();
}

int AudioStateData::Cmd_AddAudioAsset(SoundClip *pAsset)
{
	// Returns the index the frame was inserted to
	return m_PlayListModel.Add(pAsset);
}

void AudioStateData::Cmd_RemoveAudioAsset(SoundClip *pAsset)
{
	m_PlayListModel.Remove(pAsset);
}

AudioModel::AudioModel(ProjectItemData &itemRef, FileDataPair &itemFileDataRef) :
	IModel(itemRef, itemFileDataRef)
{
	InitStates<AudioStateData>(itemFileDataRef);
}

AudioModel::~AudioModel()
{
}

int AudioModel::Cmd_AddAudioAssets(int iStateIndex, QList<SoundClip *> audioAssetList)
{
	int iRow = 0;

	QList<TreeModelItemData *> depList;
	for(SoundClip *pAudio : audioAssetList)
	{
		depList.push_back(pAudio);
		iRow = static_cast<AudioStateData *>(m_StateList[iStateIndex])->Cmd_AddAudioAsset(pAudio);
	}

	depList = m_ItemRef.GetProject().IncrementDependencies(&m_ItemRef, depList);
	if(depList.size() != audioAssetList.size())
		HyGuiLog("AudioModel::Cmd_AddFrames - IncrementDependencies didn't process the entire frame list", LOGTYPE_Error);

	return iRow;
}

void AudioModel::Cmd_RemoveAudioAssets(int iStateIndex, QList<SoundClip *> audioAssetList)
{
	QList<TreeModelItemData *> depList;
	for(SoundClip *pAudio : audioAssetList)
	{
		depList.push_back(pAudio);
		static_cast<AudioStateData *>(m_StateList[iStateIndex])->Cmd_RemoveAudioAsset(pAudio);
	}

	m_ItemRef.GetProject().DecrementDependencies(&m_ItemRef, depList);
}

AudioPlayListModel &AudioModel::GetPlayListModel(uint uiStateIndex)
{
	return static_cast<AudioStateData *>(m_StateList[uiStateIndex])->GetPlayListModel();
}

PropertiesTreeModel &AudioModel::GetPropertiesModel(uint uiStateIndex)
{
	return static_cast<AudioStateData *>(m_StateList[uiStateIndex])->GetPropertiesModel();
}

/*virtual*/ void AudioModel::OnPopState(int iPoppedStateIndex) /*override*/
{
	AudioStateData *pState = static_cast<AudioStateData *>(m_StateList[iPoppedStateIndex]);
	m_ItemRef.GetProject().DecrementDependencies(&m_ItemRef, pState->GetSoundClips());
}

/*virtual*/ bool AudioModel::OnPrepSave() /*override*/
{
	return true;
}

/*virtual*/ void AudioModel::InsertItemSpecificData(FileDataPair &itemSpecificFileDataOut) /*override*/
{
}

/*virtual*/ void AudioModel::InsertStateSpecificData(uint32 uiIndex, FileDataPair &stateFileDataOut) const /*override*/
{
	AudioStateData *pState = static_cast<AudioStateData *>(m_StateList[uiIndex]);
	AudioPlayListModel &playListModelRef = pState->GetPlayListModel();
	PropertiesTreeModel &propertiesModelRef = pState->GetPropertiesModel();

	// META ////////////////////////////////////////////////////////////////////////
	QList<IAssetItemData *> assetList;
	for(int i = 0; i < playListModelRef.rowCount(); ++i)
		assetList << playListModelRef.GetPlayListItemAt(i)->GetAudioAsset();
	QJsonArray assetUuidArray;
	for(int i = 0; i < assetList.size(); ++i)
		assetUuidArray.append(assetList[i]->GetUuid().toString(QUuid::WithoutBraces));
	stateFileDataOut.m_Meta.insert("assetUUIDs", assetUuidArray);

	// DATA ////////////////////////////////////////////////////////////////////////
	stateFileDataOut.m_Data.insert("playlist", pState->GenPlayListArray());
	stateFileDataOut.m_Data.insert("playlistMode", propertiesModelRef.FindPropertyValue("Play List", "Mode").toInt());
	stateFileDataOut.m_Data.insert("volume", propertiesModelRef.FindPropertyValue("Mixing", "Volume").toDouble());
	stateFileDataOut.m_Data.insert("pitch", propertiesModelRef.FindPropertyValue("Mixing", "Pitch").toDouble());
	stateFileDataOut.m_Data.insert("priority", propertiesModelRef.FindPropertyValue("Mixing", "Priority").toInt());
	if(propertiesModelRef.IsCategoryEnabled("Looping"))
		stateFileDataOut.m_Data.insert("loops", propertiesModelRef.FindPropertyValue("Looping", "Num Loops").toInt());
	else
		stateFileDataOut.m_Data.insert("loops", 0);
	if(propertiesModelRef.IsCategoryEnabled("Positional"))
		stateFileDataOut.m_Data.insert("maxDist", propertiesModelRef.FindPropertyValue("Positional", "Max Distance").toInt());
	else
		stateFileDataOut.m_Data.insert("maxDist", 0);
}

/*virtual*/ void AudioModel::OnItemDeleted() /*override*/
{
	// No item-specific resources to delete
}
