/**************************************************************************
 *	AtlasFrame.cpp
 *
 *	Harmony Engine - Editor Tool
 *	Copyright (c) 2016 Jason Knobler
 *
 *	Harmony Editor Tool License:
 *	https://github.com/GameOverture/HarmonyEngine/blob/master/LICENSE
 *************************************************************************/
#include "Global.h"
#include "AtlasFrame.h"
#include "IManagerModel.h"
#include "_Dependencies/scriptum/imagepacker.h"
#include "SpineModel.h"

AtlasFrame::AtlasFrame(IManagerModel &modelRef,
					   ItemType eType,
					   QUuid uuid,
					   quint32 uiChecksum,
					   quint32 uiBankId,
					   QString sName,
					   QRect rAlphaCrop,
					   HyTextureInfo texInfo,
					   int iW,
					   int iH,
					   int iX,
					   int iY,
					   int iTextureIndex,
					   uint uiErrors) :
	IAssetItemData(modelRef, eType, uuid, uiChecksum, uiBankId, sName, ".png", uiErrors),
	m_iWidth(iW),
	m_iHeight(iH),
	m_rAlphaCrop(rAlphaCrop),
	m_TexInfo(texInfo),
	m_iTextureIndex(iTextureIndex),
	m_iPosX(iX),
	m_iPosY(iY)
{
}

AtlasFrame::~AtlasFrame()
{
}

QSize AtlasFrame::GetSize() const
{
	return QSize(m_iWidth, m_iHeight);
}

QRect AtlasFrame::GetCrop() const
{
	return m_rAlphaCrop;
}

HyTextureFormat AtlasFrame::GetFormat() const
{
	return m_TexInfo.GetFormat();
}

void AtlasFrame::SetFormat(HyTextureFormat eFormat, uint8 uiFormatParam1, uint8 uiFormatParam2)
{
	m_TexInfo.m_uiFormat = eFormat;
	m_TexInfo.m_uiFormatParam1 = uiFormatParam1;
	m_TexInfo.m_uiFormatParam2 = uiFormatParam2;
}

HyTextureFiltering AtlasFrame::GetFiltering() const
{
	return m_TexInfo.GetFiltering();
}

void AtlasFrame::SetFiltering(HyTextureFiltering eFiltering)
{
	m_TexInfo.m_uiFiltering = eFiltering;
}

HyTextureInfo AtlasFrame::GetTextureInfo() const
{
	return m_TexInfo;
}

QPoint AtlasFrame::GetPosition() const
{
	return QPoint(m_iPosX, m_iPosY);
}

int AtlasFrame::GetTextureIndex() const
{
	return m_iTextureIndex;
}

int AtlasFrame::GetX() const
{
	return m_iPosX;
}

int AtlasFrame::GetY() const
{
	return m_iPosY;
}

QIcon &AtlasFrame::GetThumbnail()
{
	if(m_Thumbnail.isNull())
		m_Thumbnail = QIcon(GetAbsMetaFilePath());

	return m_Thumbnail;
}

void AtlasFrame::UpdateInfoFromPacker(int iTextureIndex, int iX, int iY, QSize fullAtlasSize)
{
	m_iTextureIndex = iTextureIndex;
	m_iPosX = iX;
	m_iPosY = iY;

	if(m_iTextureIndex != -1)
	{
		ClearError(ASSETERROR_CouldNotPack);
		
		// If this is a 'Spine' type, it means it is a Sub-Atlas that was just packed into a larger texture.
		// Update the corresponding .atlas file
		if(m_eTYPE == ITEM_Spine)
		{
			QList<TreeModelItemData *> dependantList = GetDependants();
			for(auto iter = dependantList.begin(); iter != dependantList.end(); ++iter) // There should only be '1' dependency
			{
				if((*iter)->IsProjectItem() == false)
					continue;

				ProjectItemData *pProjItem = static_cast<ProjectItemData *>(*iter);
				static_cast<SpineModel *>(pProjItem->GetModel())->RewriteAtlasFile(this, fullAtlasSize);
			}
		}
	}
	else
		SetError(ASSETERROR_CouldNotPack);
}

void AtlasFrame::ReplaceImage(QString sName, quint32 uiChecksum, QImage &newImage, QDir metaDir)
{
	m_sName = sName;

	m_uiChecksum = uiChecksum;
	m_iWidth = newImage.width();
	m_iHeight = newImage.height();

	if(m_eTYPE == ITEM_AtlasFrame)
		m_rAlphaCrop = ImagePacker::crop(newImage);
	else // 'sub-atlases' should not be cropping their alpha because they rely on their own UV coordinates
		m_rAlphaCrop = QRect(0, 0, newImage.width(), newImage.height());

	// DO NOT clear 'm_iTextureIndex' as it's needed in the Repack()

	if(newImage.save(metaDir.path() % "/" % ConstructMetaFileName()) == false)
		HyGuiLog("Could not save frame image to meta directory: " % m_sName, LOGTYPE_Error);

	m_Thumbnail = QIcon(); // Reset thumbnail
}

/*virtual*/ QString AtlasFrame::GetPropertyInfo() /*override*/
{
	return QString(HyAssets::GetTextureFormatName(static_cast<HyTextureFormat>(m_TexInfo.m_uiFormat)).c_str()) % " | " % QString(HyAssets::GetTextureFilteringName(static_cast<HyTextureFiltering>(m_TexInfo.m_uiFiltering)).c_str());
}

/*virtual*/ void AtlasFrame::InsertUniqueJson(QJsonObject &frameObj) /*override*/
{
	frameObj.insert("width", QJsonValue(GetSize().width()));
	frameObj.insert("height", QJsonValue(GetSize().height()));
	frameObj.insert("textureIndex", QJsonValue(GetTextureIndex()));
	frameObj.insert("x", QJsonValue(GetX()));
	frameObj.insert("y", QJsonValue(GetY()));
	frameObj.insert("cropLeft", QJsonValue(GetCrop().left()));
	frameObj.insert("cropTop", QJsonValue(GetCrop().top()));
	frameObj.insert("cropRight", QJsonValue(GetCrop().right()));
	frameObj.insert("cropBottom", QJsonValue(GetCrop().bottom()));
	frameObj.insert("textureInfo", QJsonValue(static_cast<qint64>(m_TexInfo.GetBucketId())));
}
