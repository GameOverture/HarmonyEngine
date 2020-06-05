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

#include "IManagerModel.h"
#include "ProjectItemData.h"

#include <QWidget>
#include <QSet>
#include <QJsonObject>
#include <QDataStream>

//class AtlasTreeItem;

class AtlasFrame : public AssetItemData
{
	//friend class AtlasModel;
	
	AtlasItemType						m_eType;

	int									m_iWidth;
	int									m_iHeight;
	QRect								m_rAlphaCrop;

	int									m_iTextureIndex;

	int									m_iPosX;
	int									m_iPosY;

public:
	AtlasFrame(QUuid uuid, quint32 uiChecksum, quint32 uiBankId, QString sName, QRect rAlphaCrop, AtlasItemType eType, int iW, int iH, int iX, int iY, int iTextureIndex, uint uiErrors);
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

	void GetJsonObj(QJsonObject &frameObj);

private:
	//void UpdateTreeItemIconAndToolTip();
	bool DeleteMetaImage(QDir metaDir);
	void ReplaceImage(QString sName, quint32 uiChecksum, QImage &newImage, QDir metaDir);
};
//Q_DECLARE_METATYPE(AtlasFrame *)
//
//QDataStream &operator<<(QDataStream &out, AtlasFrame *const &rhs);
//QDataStream &operator>>(QDataStream &in, AtlasFrame *rhs);

#endif // ATLASFRAME_H
