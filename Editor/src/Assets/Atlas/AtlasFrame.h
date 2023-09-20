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

	bool								m_bIsSubAtlas;

	int									m_iWidth;
	int									m_iHeight;
	QRect								m_rAlphaCrop;

	HyTextureInfo						m_TexInfo;
	int									m_iTextureIndex;

	int									m_iPosX;
	int									m_iPosY;

	QIcon								m_Thumbnail;

public:
	AtlasFrame(IManagerModel &modelRef,
			   bool bIsSubAtlas,
			   QUuid uuid,
			   quint32 uiChecksum,
			   quint32 uiBankId,
			   QString sName,
			   QRect rAlphaCrop,
			   HyTextureInfo texInfo,
			   int iW, int iH, int iX, int iY,
			   int iTextureIndex,
			   uint uiErrors);
	~AtlasFrame();

	bool IsSubAtlas() const;

	QSize GetSize() const;
	QRect GetCrop() const;
	
	HyTextureFormat GetFormat() const;
	void SetFormat(HyTextureFormat eFormat, uint8 uiFormatParam1, uint8 uiFormatParam2);

	HyTextureFiltering GetFiltering() const;
	void SetFiltering(HyTextureFiltering eFiltering);

	HyTextureInfo GetTextureInfo() const;

	QPoint GetPosition() const;

	int GetTextureIndex() const;
	int GetX() const;
	int GetY() const;

	QIcon &GetThumbnail();

	void ClearTextureIndex();
	void UpdateInfoFromPacker(int iTextureIndex, int iX, int iY, QSize textureSize);
	void ReplaceImage(QString sName, quint32 uiChecksum, QImage &newImage, bool bIsSubAtlas, QDir metaDir);

	virtual QString GetPropertyInfo() override;
	virtual void InsertUniqueJson(QJsonObject &frameObj) override;
};

#endif // ATLASFRAME_H
