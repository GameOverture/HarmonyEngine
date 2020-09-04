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

AudioStateData::AudioStateData(int iStateIndex, IModel &modelRef, FileDataPair stateFileData) :
	IStateData(iStateIndex, modelRef, stateFileData)
{
}

/*virtual*/ AudioStateData::~AudioStateData()
{
}

/*virtual*/ QVariant AudioStateData::OnLinkAsset(AssetItemData *pAsset) /*override*/
{
	return 0;
}

/*virtual*/ void AudioStateData::OnUnlinkAsset(AssetItemData *pAsset) /*override*/
{
}

AudioModel::AudioModel(ProjectItemData &itemRef, FileDataPair &itemFileDataRef) :
	IModel(itemRef, itemFileDataRef),
	m_PropertiesModel(itemRef, -1, -1, this)
{
	InitStates<AudioStateData>(itemFileDataRef);

	QStringList sPlayListModeList;
	sPlayListModeList << QObject::tr("Shuffle");
	sPlayListModeList << QObject::tr("Weighted");
	sPlayListModeList << QObject::tr("Sequential (Local)");
	sPlayListModeList << QObject::tr("Sequential (Global)");

	//itemFileDataRef.m_Meta
	const int iRANGE = 16777215;	// Uses 3 bytes (0xFFFFFF)... Qt uses this value for their default ranges in QSpinBox

	m_PropertiesModel.AppendCategory("Play List");
	m_PropertiesModel.AppendProperty("Play List", "Mode", PROPERTIESTYPE_ComboBox, 0, "The method by which the next sample is chosen when this item is played", false, QVariant(), QVariant(), QVariant(), QString(), QString(), sPlayListModeList);

	m_PropertiesModel.AppendCategory("Mixing");
	m_PropertiesModel.AppendProperty("Mixing", "Volume", PROPERTIESTYPE_double, 1.0, "Volume modifier [0.0 to 1.0] applied to source volume", false, 0.0, 1.0, 0.1, QString(), QString(), 4);
	m_PropertiesModel.AppendProperty("Mixing", "Pitch", PROPERTIESTYPE_double, 1.0, "Pitch modifier [0.0 to 1.0] applied to source volume", false, 0.0, 1.0, 0.1, QString(), QString(), 4);
	m_PropertiesModel.AppendProperty("Mixing", "Priority", PROPERTIESTYPE_int, 0, "Samples with higher priority will take presidence over others when channels are maxed out", false, -iRANGE, iRANGE, 1);

	m_PropertiesModel.AppendCategory("Looping", QVariant(), true, false, "The sample will play for loops+1 number of times, until stopped");
	m_PropertiesModel.AppendProperty("Looping", "Num Loops", PROPERTIESTYPE_int, -1, "Number of loops, -1 is infinite loops. Passing one here plays the sample twice (1 loop)", false, -1);

	m_PropertiesModel.AppendCategory("Positional", QVariant(), true, false, "Emulates a simple 3D audio effect based on the item's position relative to the listening camera");
	m_PropertiesModel.AppendProperty("Positional", "Max Distance", PROPERTIESTYPE_int, 0, "The distance (and beyond) at which the sample will be played at its quietest", false, 0, iRANGE, 1);
}

AudioModel::~AudioModel()
{
}

/*virtual*/ bool AudioModel::OnPrepSave() /*override*/
{
	return true;
}

/*virtual*/ void AudioModel::InsertItemSpecificData(FileDataPair &itemSpecificFileDataOut) /*override*/
{
}

/*virtual*/ FileDataPair AudioModel::GetStateFileData(uint32 uiIndex) const /*override*/
{
	return FileDataPair();
}

/*virtual*/ QList<AssetItemData *> AudioModel::GetAssets(HyGuiItemType eType) const /*override*/
{
	return QList<AssetItemData *>();
}

/*virtual*/ QStringList AudioModel::GetFontUrls() const /*override*/
{
	return QStringList();
}
