/**************************************************************************
 *	AtlasFrame.h
 *
 *	Harmony Engine - Editor Tool
 *	Copyright (c) 2016 Jason Knobler
 *
 *	Harmony Editor Tool License:
 *	https://github.com/GameOverture/HarmonyEngine/blob/master/LICENSE
 *************************************************************************/
#ifndef ATLASFRAME_H
#define ATLASFRAME_H

#include "IAssetItemData.h"
#include "ProjectItemData.h"

#include <QWidget>
#include <QSet>
#include <QJsonObject>
#include <QDataStream>

class AtlasFrame : public IAssetItemData
{
	Q_OBJECT

	ItemType							m_eSubAtlasType;

	quint16								m_uiWidth;
	quint16								m_uiHeight;
	
	quint16								m_uiCropLeft;
	quint16								m_uiCropTop;
	quint16								m_uiCropRight;
	quint16								m_uiCropBottom;

	HyTextureInfo						m_TexInfo;
	int									m_iTextureIndex;

	quint16								m_uiPosX;
	quint16								m_uiPosY;

	QIcon								m_Thumbnail;

public:
	AtlasFrame(IManagerModel &modelRef,
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
			   quint16 uiW, quint16 uiH, quint16 uiX, quint16 uiY,
			   int iTextureIndex,
			   uint uiErrors);
	~AtlasFrame();

	ItemType GetSubAtlasType() const;

	QSize GetSize() const;	// This is the size of the original import (with alpha)
	quint16 GetCropL() const;
	quint16 GetCropT() const;
	quint16 GetCropR() const;
	quint16 GetCropB() const;
	quint16 GetCroppedWidth() const;
	quint16 GetCroppedHeight() const;
	quint64 GetCropMask() const;
	quint64 GetFrameMask() const;
	
	HyTextureFormat GetFormat() const;
	void SetFormat(HyTextureFormat eFormat, uint8 uiFormatParam1, uint8 uiFormatParam2);

	HyTextureFiltering GetFiltering() const;
	void SetFiltering(HyTextureFiltering eFiltering);

	HyTextureInfo GetTextureInfo() const;

	QPoint GetPosition() const;

	int GetTextureIndex() const;
	quint16 GetX() const;
	quint16 GetY() const;

	QIcon &GetThumbnail();

	void ClearTextureIndex();
	void UpdateInfoFromPacker(int iTextureIndex, quint16 uiX, quint16 uiY, QSize textureSize);
	void ReplaceImage(QString sName, quint32 uiChecksum, QImage &newImage, ItemType eSubAtlasType, QDir metaDir);

	virtual QString GetPropertyInfo() override;
	virtual void InsertUniqueJson(QJsonObject &frameObj) override;
};

#endif // ATLASFRAME_H
