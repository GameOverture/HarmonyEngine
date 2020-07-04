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

AtlasFrame::AtlasFrame(IManagerModel &modelRef,
					   HyGuiItemType eType,
					   QUuid uuid,
					   quint32 uiChecksum,
					   quint32 uiBankId,
					   QString sName,
					   QRect rAlphaCrop,
					   int iW,
					   int iH,
					   int iX,
					   int iY,
					   int iTextureIndex,
					   uint uiErrors) :
	AssetItemData(modelRef, eType, uuid, uiChecksum, uiBankId, sName, ".png", uiErrors),
	m_iWidth(iW),
	m_iHeight(iH),
	m_rAlphaCrop(rAlphaCrop),
	m_iPosX(iX),
	m_iPosY(iY),
	m_iTextureIndex(iTextureIndex)
{
}

AtlasFrame::~AtlasFrame()
{
}

QSize AtlasFrame::GetSize()
{
	return QSize(m_iWidth, m_iHeight);
}

QRect AtlasFrame::GetCrop()
{
	return m_rAlphaCrop;
}

QPoint AtlasFrame::GetPosition()
{
	return QPoint(m_iPosX, m_iPosY);
}

int AtlasFrame::GetTextureIndex()
{
	return m_iTextureIndex;
}

int AtlasFrame::GetX()
{
	return m_iPosX;
}

int AtlasFrame::GetY()
{
	return m_iPosY;
}

void AtlasFrame::UpdateInfoFromPacker(int iTextureIndex, int iX, int iY)
{
	m_iTextureIndex = iTextureIndex;
	m_iPosX = iX;
	m_iPosY = iY;

	if(m_iTextureIndex != -1)
	{
		ClearError(ATLASFRAMEERROR_CouldNotPack);

		//if(m_pTreeWidgetItem)
		//	m_pTreeWidgetItem->setText(1, "Id:" % QString::number(m_uiAtlasGrpId));
	}
	else
	{
		SetError(ATLASFRAMEERROR_CouldNotPack);
		//if(m_pTreeWidgetItem)
		//	m_pTreeWidgetItem->setText(1, "Invalid");
	}
}

void AtlasFrame::ReplaceImage(QString sName, quint32 uiChecksum, QImage &newImage, QDir metaDir)
{
	m_sName = sName;

	//if(m_pTreeWidgetItem)
	//	m_pTreeWidgetItem->setText(0, m_sName);

	m_uiChecksum = uiChecksum;
	m_iWidth = newImage.width();
	m_iHeight = newImage.height();

	if(m_eTYPE == ITEM_AtlasImage)
		m_rAlphaCrop = ImagePacker::crop(newImage);
	else // 'sub-atlases' should not be cropping their alpha because they rely on their own UV coordinates
		m_rAlphaCrop = QRect(0, 0, newImage.width(), newImage.height());

	// DO NOT clear 'm_iTextureIndex' as it's needed in the Repack()

	if(newImage.save(metaDir.path() % "/" % ConstructMetaFileName()) == false)
		HyGuiLog("Could not save frame image to meta directory: " % m_sName, LOGTYPE_Error);
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
}
