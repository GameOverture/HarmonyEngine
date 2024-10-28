/**************************************************************************
 *	AtlasTileSet.cpp
 *
 *	Harmony Engine - Editor Tool
 *	Copyright (c) 2024 Jason Knobler
 *
 *	Harmony Editor Tool License:
 *	https://github.com/GameOverture/HarmonyEngine/blob/master/LICENSE
 *************************************************************************/
#include "Global.h"
#include "AtlasTileSet.h"

AtlasTileSet::AtlasTileSet(IManagerModel &modelRef,
					   QUuid uuid,
					   quint32 uiChecksum,
					   quint32 uiBankId,
					   QString sName,
					   HyTextureInfo texInfo,
					   quint16 uiW,
					   quint16 uiH,
					   quint16 uiX,
					   quint16 uiY,
					   int iTextureIndex,
					   uint uiErrors) :
	AtlasFrame(ITEM_AtlasTileSet, modelRef, ITEM_AtlasTileSet, uuid, uiChecksum, uiBankId, sName, 0, 0, 0, 0, texInfo, uiW, uiH, uiX, uiY, iTextureIndex, uiErrors)
{
}

AtlasTileSet::~AtlasTileSet()
{
}

/*virtual*/ void AtlasTileSet::InsertUniqueJson(QJsonObject &frameObj) /*override*/
{
	frameObj.insert("subAtlasType", QJsonValue(HyGlobal::ItemName(m_eSubAtlasType, false)));
	frameObj.insert("width", QJsonValue(GetSize().width()));
	frameObj.insert("height", QJsonValue(GetSize().height()));
	frameObj.insert("textureIndex", QJsonValue(GetTextureIndex()));
	frameObj.insert("x", QJsonValue(GetX()));
	frameObj.insert("y", QJsonValue(GetY()));
	frameObj.insert("cropLeft", QJsonValue(m_uiCropLeft));// GetCrop().left()));
	frameObj.insert("cropTop", QJsonValue(m_uiCropTop));//GetCrop().top()));
	frameObj.insert("cropRight", QJsonValue(m_uiCropRight));//GetCrop().right()));
	frameObj.insert("cropBottom", QJsonValue(m_uiCropBottom));//GetCrop().bottom()));
	frameObj.insert("textureInfo", QJsonValue(static_cast<qint64>(m_TexInfo.GetBucketId())));
}
