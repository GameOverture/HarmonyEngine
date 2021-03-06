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
	QJsonArray dataPlayListArray = stateFileData.m_Data["playList"].toArray();

	QList<QUuid> uuidRequestList;
	for(int i = 0; i < metaAssetArray.size(); ++i)
		uuidRequestList.append(QUuid(metaAssetArray[i].toString()));

	QList<AssetItemData *> requestedAtlasFramesList = m_ModelRef.GetItem().GetProject().GetAudioModel().RequestAssetsByUuid(&m_ModelRef.GetItem(), uuidRequestList);
	for(int i = 0; i < requestedAtlasFramesList.size(); ++i)
		OnLinkAsset(requestedAtlasFramesList[i]);

	if(dataPlayListArray.size() != requestedAtlasFramesList.size())
		HyGuiLog("SpriteStatesModel::AppendState() failed to acquire all the stored frames", LOGTYPE_Error);

	// NOTE: This 'sPlayListModeList' order must be preserved as it is saved as an index
	QStringList sPlayListModeList;
	sPlayListModeList << QObject::tr("Shuffle");
	sPlayListModeList << QObject::tr("Weighted");
	sPlayListModeList << QObject::tr("Sequential (Local)");
	sPlayListModeList << QObject::tr("Sequential (Global)");

	
	const int iRANGE = 16777215;	// Uses 3 bytes (0xFFFFFF)... Qt uses this value for their default ranges in QSpinBox

	m_PropertiesModel.AppendCategory("Play List");
	m_PropertiesModel.AppendProperty("Play List", "Mode", PROPERTIESTYPE_ComboBox, stateFileData.m_Data["playListMode"].toInt(0), "The method by which the next audio asset is chosen when this item is played", false, QVariant(), QVariant(), QVariant(), QString(), QString(), sPlayListModeList);

	m_PropertiesModel.AppendCategory("Mixing");
	m_PropertiesModel.AppendProperty("Mixing", "Volume", PROPERTIESTYPE_double, stateFileData.m_Data["volume"].toDouble(1.0), "Volume modifier [0.0 to 1.0] applied to source volume", false, 0.0, 1.0, 0.1, QString(), QString(), 4);
	m_PropertiesModel.AppendProperty("Mixing", "Pitch", PROPERTIESTYPE_double, stateFileData.m_Data["pitch"].toDouble(1.0), "Pitch modifier [0.0 to 1.0] applied to source volume", false, 0.0, 1.0, 0.1, QString(), QString(), 4);
	m_PropertiesModel.AppendProperty("Mixing", "Priority", PROPERTIESTYPE_int, stateFileData.m_Data["priority"].toInt(0), "Audio assets with higher priority will take presidence over others when channels are maxed out", false, -iRANGE, iRANGE, 1);

	int iLoops = stateFileData.m_Data["loops"].toInt(0);
	m_PropertiesModel.AppendCategory("Looping", QVariant(), true, iLoops != 0, "The audio asset will play for loops+1 number of times, until stopped");
	m_PropertiesModel.AppendProperty("Looping", "Num Loops", PROPERTIESTYPE_int, iLoops != 0 ? iLoops : -1, "Number of loops, -1 is infinite loops. Passing one here plays the audio asset twice (1 loop)", false, -1);

	int iDist = stateFileData.m_Data["maxDist"].toInt(0);
	m_PropertiesModel.AppendCategory("Positional", QVariant(), true, iDist != 0, "Emulates a simple 3D audio effect based on the item's position relative to the listening camera");
	m_PropertiesModel.AppendProperty("Positional", "Max Distance", PROPERTIESTYPE_int, iDist != 0 ? iDist : 1, "The distance (and beyond) at which the audio asset will be played at its quietest", false, 0, iRANGE, 1);
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

QJsonArray AudioStateData::GenPlayListArray() const
{
	return m_PlayListModel.GenPlayListArray();
}

/*virtual*/ QVariant AudioStateData::OnLinkAsset(AssetItemData *pAsset) /*override*/
{
	if(pAsset->GetType() != ITEM_Audio)
		HyGuiLog("AudioStateData::OnLinkAsset linked non Audio asset", LOGTYPE_Error);

	// Returns the index the frame was inserted to
	return m_PlayListModel.Add(static_cast<AudioAsset *>(pAsset));
}

/*virtual*/ void AudioStateData::OnUnlinkAsset(AssetItemData *pAsset) /*override*/
{
	if(pAsset->GetType() != ITEM_Audio)
		HyGuiLog("AudioStateData::OnUnlinkAsset unlinked non Audio asset", LOGTYPE_Error);

	m_PlayListModel.Remove(static_cast<AudioAsset *>(pAsset));
}

AudioModel::AudioModel(ProjectItemData &itemRef, FileDataPair &itemFileDataRef) :
	IModel(itemRef, itemFileDataRef)
{
	InitStates<AudioStateData>(itemFileDataRef);
}

AudioModel::~AudioModel()
{
}

AudioPlayListModel &AudioModel::GetPlayListModel(uint uiStateIndex)
{
	return static_cast<AudioStateData *>(m_StateList[uiStateIndex])->GetPlayListModel();
}

PropertiesTreeModel &AudioModel::GetPropertiesModel(uint uiStateIndex)
{
	return static_cast<AudioStateData *>(m_StateList[uiStateIndex])->GetPropertiesModel();
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
	QList<AssetItemData *> assetList;
	for(int i = 0; i < playListModelRef.rowCount(); ++i)
		assetList << playListModelRef.GetAudioAssetAt(i)->GetAudioAsset();
	QJsonArray assetUuidArray;
	for(int i = 0; i < assetList.size(); ++i)
		assetUuidArray.append(assetList[i]->GetUuid().toString(QUuid::WithoutBraces));
	stateFileDataOut.m_Meta.insert("assetUUIDs", assetUuidArray);

	// DATA ////////////////////////////////////////////////////////////////////////
	stateFileDataOut.m_Data.insert("playList", pState->GenPlayListArray());
	stateFileDataOut.m_Data.insert("playListMode", propertiesModelRef.FindPropertyValue("Play List", "Mode").toInt());
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

/*virtual*/ QList<AssetItemData *> AudioModel::GetAssets(HyGuiItemType eType) const /*override*/
{
	if(eType != ITEM_Audio)
		return QList<AssetItemData *>();

	QList<AssetItemData *> returnList;
	for(auto pState : m_StateList)
	{
		for(int i = 0; i < static_cast<AudioStateData *>(pState)->GetPlayListModel().rowCount(); ++i)
			returnList << static_cast<AudioStateData *>(pState)->GetPlayListModel().GetAudioAssetAt(i)->GetAudioAsset();
	}

	return returnList;
}

/*virtual*/ QStringList AudioModel::GetFontUrls() const /*override*/
{
	return QStringList();
}
