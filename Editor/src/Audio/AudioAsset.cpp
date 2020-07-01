/**************************************************************************
 *	AudioAsset.cpp
 *
 *	Harmony Engine - Editor Tool
 *	Copyright (c) 2020 Jason Knobler
 *
 *	Harmony Editor Tool License:
 *	https://github.com/GameOverture/HarmonyEngine/blob/master/LICENSE
 *************************************************************************/
#include "Global.h"
#include "AudioAsset.h"
#include "IManagerModel.h"
#include "_Dependencies/scriptum/imagepacker.h"

AudioAsset::AudioAsset(IManagerModel &modelRef,
					   HyGuiItemType eType,
					   QUuid uuid,
					   quint32 uiChecksum,
					   quint32 uiBankId,
					   QString sName,
					   QString sFormat,
					   uint uiErrors) :
	AssetItemData(modelRef, eType, uuid, uiChecksum, uiBankId, sName, ".wav", uiErrors)
{
}

AudioAsset::~AudioAsset()
{
}

void AudioAsset::ReplaceAudio(QString sName, uint32 uiChecksum)
{
	m_sName = sName;
	m_uiChecksum = uiChecksum;
}

/*virtual*/ void AudioAsset::InsertUniqueJson(QJsonObject &frameObj) /*override*/
{

}
