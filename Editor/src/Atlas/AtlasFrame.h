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

class AtlasFrame : public AssetItemData
{
	int									m_iWidth;
	int									m_iHeight;
	QRect								m_rAlphaCrop;

	int									m_iTextureIndex;

	int									m_iPosX;
	int									m_iPosY;

public:
	AtlasFrame(IManagerModel &modelRef, HyGuiItemType eType, QUuid uuid, quint32 uiChecksum, quint32 uiBankId, QString sName, QRect rAlphaCrop, int iW, int iH, int iX, int iY, int iTextureIndex, uint uiErrors);
	~AtlasFrame();

	//AtlasTreeItem *GetTreeItem();
	QSize GetSize();
	QRect GetCrop();
	QPoint GetPosition();
	AtlasItemType GetType();

	int GetTextureIndex();
	int GetX();
	int GetY();

	void UpdateInfoFromPacker(int iTextureIndex, int iX, int iY);

	virtual void GetJsonObj(QJsonObject &frameObj) override;

	//void UpdateTreeItemIconAndToolTip();
	void ReplaceImage(QString sName, quint32 uiChecksum, QImage &newImage, QDir metaDir);
};
//Q_DECLARE_METATYPE(AtlasFrame *)
//
//QDataStream &operator<<(QDataStream &out, AtlasFrame *const &rhs);
//QDataStream &operator>>(QDataStream &in, AtlasFrame *rhs);

#endif // ATLASFRAME_H
