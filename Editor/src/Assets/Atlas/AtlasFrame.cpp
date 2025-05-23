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
#include "SpineModel.h"

AtlasFrame::AtlasFrame(ItemType eThisAssetType, // Might be either ITEM_AtlasFrame or ITEM_AtlasTileSet
					   IManagerModel &modelRef,
					   ItemType eSubAtlasType,
					   QUuid uuid,
					   quint32 uiChecksum,
					   quint32 uiBankId,
					   QString sName,
					   quint16 uiCropLeft,
					   quint16 uiCropTop,
					   quint16 uiCropRight,
					   quint16 uiCropBottom,
					   HyTextureInfo texInfo,
					   quint16 uiW,
					   quint16 uiH,
					   quint16 uiX,
					   quint16 uiY,
					   int iTextureIndex,
					   uint uiErrors) :
	IAssetItemData(modelRef, eThisAssetType, uuid, uiChecksum, uiBankId, sName, ".png", uiErrors),
	m_eSubAtlasType(eSubAtlasType),
	m_uiWidth(uiW),
	m_uiHeight(uiH),
	m_uiCropLeft(uiCropLeft),
	m_uiCropTop(uiCropTop),
	m_uiCropRight(uiCropRight),
	m_uiCropBottom(uiCropBottom),
	m_TexInfo(texInfo),
	m_iTextureIndex(iTextureIndex),
	m_uiPosX(uiX),
	m_uiPosY(uiY)
{
}

AtlasFrame::~AtlasFrame()
{
}

ItemType AtlasFrame::GetSubAtlasType() const
{
	return m_eSubAtlasType;
}

QSize AtlasFrame::GetSize() const
{
	return QSize(m_uiWidth, m_uiHeight);
}

quint16 AtlasFrame::GetCropL() const
{
	return m_uiCropLeft;
}

quint16 AtlasFrame::GetCropT() const
{
	return m_uiCropTop;
}

quint16 AtlasFrame::GetCropR() const
{
	return m_uiCropRight;
}

quint16 AtlasFrame::GetCropB() const
{
	return m_uiCropBottom;
}

quint16 AtlasFrame::GetCroppedWidth() const
{
	return m_uiWidth - m_uiCropLeft - m_uiCropRight;
}

quint16 AtlasFrame::GetCroppedHeight() const
{
	return m_uiHeight - m_uiCropTop - m_uiCropBottom;
}

// LEFT, TOP, WIDTH, HEIGHT = 16 bits each. Laid out like: 0xLLLLTTTTRRRRBBBBB
quint64 AtlasFrame::GetCropMask() const
{
	return (quint64(m_uiCropLeft) << 48) | (quint64(m_uiCropTop) << 32) | (quint64(m_uiCropRight) << 16) | quint64(m_uiCropBottom);
}

// LEFT, TOP, RIGHT, BOTTOM = 16 bits each. Laid out like: 0xLLLLTTTTRRRRBBBB
quint64 AtlasFrame::GetFrameMask() const
{
	return (quint64(m_uiPosX) << 48) | (quint64(m_uiPosY) << 32) | (quint64(m_uiPosX + GetCroppedWidth()) << 16) | quint64(m_uiPosY + GetCroppedHeight());
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
	return QPoint(m_uiPosX, m_uiPosY);
}

int AtlasFrame::GetTextureIndex() const
{
	return m_iTextureIndex;
}

quint16 AtlasFrame::GetX() const
{
	return m_uiPosX;
}

quint16 AtlasFrame::GetY() const
{
	return m_uiPosY;
}

QIcon &AtlasFrame::GetThumbnail()
{
	if(m_Thumbnail.isNull())
		m_Thumbnail = QIcon(GetAbsMetaFilePath());

	return m_Thumbnail;
}

void AtlasFrame::ClearTextureIndex()
{
	m_iTextureIndex = -1;
}

void AtlasFrame::UpdateInfoFromPacker(int iTextureIndex, quint16 uiX, quint16 uiY, QSize textureSize)
{
	m_iTextureIndex = iTextureIndex;
	m_uiPosX = uiX;
	m_uiPosY = uiY;

	if(m_iTextureIndex != -1)
	{
		ClearError(ASSETERROR_CouldNotPack);
		
		//// If this is a 'Spine' type, it means it is a Sub-Atlas that was just packed into a larger texture.
		//// Update the corresponding .atlas file
		//if(m_eTYPE == ITEM_Spine)
		//{
		//	QList<TreeModelItemData *> dependantList = GetDependants();
		//	for(auto iter = dependantList.begin(); iter != dependantList.end(); ++iter) // There should only be '1' dependency
		//	{
		//		if((*iter)->IsProjectItem() == false)
		//			continue;

		//		ProjectItemData *pProjItem = static_cast<ProjectItemData *>(*iter);
		//		static_cast<SpineModel *>(pProjItem->GetModel())->RewriteAtlasFile(this, textureSize);
		//	}
		//}
	}
	else
		SetError(ASSETERROR_CouldNotPack);
}

void AtlasFrame::ReplaceImage(QString sName, quint32 uiChecksum, QImage &newImage, ItemType eSubAtlasType, QDir metaDir)
{
	if(eSubAtlasType == ITEM_Unknown) // NOTE: ITEM_None is valid
	{
		HyGuiLog("AtlasFrame::ReplaceImage() - eSubAtlasType is ITEM_Unknown", LOGTYPE_Error);
		return;
	}

	m_eSubAtlasType = eSubAtlasType;
	m_sName = sName;

	m_uiChecksum = uiChecksum;
	
	if(newImage.width() > std::numeric_limits<quint16>::max() || newImage.height() > std::numeric_limits<quint16>::max())
		HyGuiLog("Image dimensions are too large: " % QString::number(newImage.width()) % "x" % QString::number(newImage.height()), LOGTYPE_Error);
	m_uiWidth = newImage.width();
	m_uiHeight = newImage.height();

	QRect rAlphaCrop; 
	if(m_eSubAtlasType == ITEM_None)
		rAlphaCrop = HyGlobal::AlphaCropImage(newImage);
	else // 'sub-atlases' should not be cropping their alpha because they rely on their own UV coordinates
		rAlphaCrop = QRect(0, 0, newImage.width(), newImage.height());

	// NOTE: QRect (rAlphaCrop) needs to be converted to L,T,R,B margins
	m_uiCropLeft = rAlphaCrop.left();
	m_uiCropTop = rAlphaCrop.top();
	m_uiCropRight = newImage.width() - (rAlphaCrop.left()+rAlphaCrop.width());
	m_uiCropBottom = newImage.height() - (rAlphaCrop.top()+rAlphaCrop.height());

	// DO NOT clear 'm_iTextureIndex' as it's needed in the Repack()

	if(newImage.save(metaDir.path() % "/" % ConstructMetaFileName()) == false)
		HyGuiLog("Could not save frame image to meta directory: " % m_sName, LOGTYPE_Error);

	m_Thumbnail = QIcon(); // Reset thumbnail
}

/*virtual*/ QString AtlasFrame::GetPropertyInfo() /*override*/
{
	return QString(HyAssets::GetTextureFormatName(static_cast<HyTextureFormat>(m_TexInfo.m_uiFormat)).c_str()) % " | " % QString(HyAssets::GetTextureFilteringName(static_cast<HyTextureFiltering>(m_TexInfo.m_uiFiltering)).c_str());
}

/*virtual*/ QString AtlasFrame::OnReplaceAllowed() /*override*/
{
	if(GetSubAtlasType() != ITEM_None)
	{
		QString sDependant;
		if(GetDependants().empty() == false && GetDependants()[0]->IsProjectItem())
			sDependant = static_cast<ProjectItemData *>(GetDependants()[0])->GetName(true);
		QString sMessage;
		if(sDependant.isEmpty() == false)
			sMessage = "'" % GetName() % "' asset cannot be replaced because it is a sub-atlas of item:\n\n" % sDependant;
		else
			sMessage = "'" % GetName() % "' asset cannot be replaced because it is a sub-atlas";
		
		return sMessage;
	}

	return "";
}

/*virtual*/ void AtlasFrame::InsertUniqueJson(QJsonObject &frameObj) /*override*/
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
