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
#include "_Dependencies/scriptum/imagepacker.h"

#include "AtlasWidget.h"

AtlasFrame::AtlasFrame(IManagerModel &modelRef,
					   QUuid uuid,
					   quint32 uiChecksum,
					   quint32 uiBankId,
					   QString sName,
					   QRect rAlphaCrop,
					   AtlasItemType eType,
					   int iW,
					   int iH,
					   int iX,
					   int iY,
					   int iTextureIndex,
					   uint uiErrors) :
	AssetItemData(modelRef, uuid, uiChecksum, uiBankId, sName, ".png", uiErrors),
	m_eType(eType),
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

//AtlasTreeItem *AtlasFrame::GetTreeItem()
//{
//	if(m_pTreeWidgetItem)
//		return m_pTreeWidgetItem;
//
//	m_pTreeWidgetItem = new AtlasTreeItem((QTreeWidgetItem *)nullptr, QTreeWidgetItem::Type);
//	m_pTreeWidgetItem->setText(0, GetName());
//
//	if(m_iTextureIndex >= 0)
//	{
//		m_pTreeWidgetItem->setText(1, "Id:" % QString::number(m_uiAtlasGrpId));
//		ClearError(ATLASFRAMEERROR_CouldNotPack);
//	}
//	else
//	{
//		m_pTreeWidgetItem->setText(1, "Invalid");
//		SetError(ATLASFRAMEERROR_CouldNotPack);
//	}
//
//	UpdateTreeItemIconAndToolTip();
//
//	QVariant v; v.setValue(this);
//	m_pTreeWidgetItem->setData(0, Qt::UserRole, v);
//	m_pTreeWidgetItem->setFlags(Qt::ItemIsSelectable | Qt::ItemIsUserCheckable | Qt::ItemIsEnabled | Qt::ItemIsDragEnabled);
//
//	return m_pTreeWidgetItem;
//}



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

AtlasItemType AtlasFrame::GetType()
{
	return m_eType;
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

/*virtual*/ void AtlasFrame::GetJsonObj(QJsonObject &frameObj) /*override*/
{
	frameObj.insert("frameUUID", GetUuid().toString(QUuid::WithoutBraces));
	frameObj.insert("atlasGrpId", QJsonValue(static_cast<qint64>(GetBankId())));
	frameObj.insert("checksum", QJsonValue(static_cast<qint64>(GetChecksum())));
	frameObj.insert("name", QJsonValue(GetName()));
	frameObj.insert("width", QJsonValue(GetSize().width()));
	frameObj.insert("height", QJsonValue(GetSize().height()));
	frameObj.insert("textureIndex", QJsonValue(GetTextureIndex()));
	frameObj.insert("type", QJsonValue(GetType()));
	frameObj.insert("x", QJsonValue(GetX()));
	frameObj.insert("y", QJsonValue(GetY()));
	frameObj.insert("cropLeft", QJsonValue(GetCrop().left()));
	frameObj.insert("cropTop", QJsonValue(GetCrop().top()));
	frameObj.insert("cropRight", QJsonValue(GetCrop().right()));
	frameObj.insert("cropBottom", QJsonValue(GetCrop().bottom()));
	frameObj.insert("errors", QJsonValue(static_cast<int>(GetErrors())));

	QString sFilterPath = "";
	if(m_pTreeWidgetItem)
	{
		QTreeWidgetItem *pTreeParent = m_pTreeWidgetItem->parent();
		while(pTreeParent)
		{
			if(pTreeParent->data(0, Qt::UserRole).toString() == HYTREEWIDGETITEM_IsFilter)
				break;

			pTreeParent = pTreeParent->parent();
		}
		if(pTreeParent)
			sFilterPath = HyGlobal::GetTreeWidgetItemPath(pTreeParent);
	}

	frameObj.insert("filter", QJsonValue(sFilterPath));
}



//void AtlasFrame::UpdateTreeItemIconAndToolTip()
//{
//	if(m_pTreeWidgetItem)
//	{
//		// Duplicates are not considered and error so don't mark the icon as a warning (if only error)
//		if(m_uiErrors == 0)
//			m_pTreeWidgetItem->setIcon(0, HyGlobal::ItemIcon(HyGlobal::GetItemFromAtlasItem(m_eType), SUBICON_None));
//		else
//			m_pTreeWidgetItem->setIcon(0, HyGlobal::ItemIcon(HyGlobal::GetItemFromAtlasItem(m_eType), SUBICON_Warning));
//		
//		m_pTreeWidgetItem->setToolTip(0, HyGlobal::GetGuiFrameErrors(m_uiErrors));
//	}
//}



void AtlasFrame::ReplaceImage(QString sName, quint32 uiChecksum, QImage &newImage, QDir metaDir)
{
	m_sName = sName;

	//if(m_pTreeWidgetItem)
	//	m_pTreeWidgetItem->setText(0, m_sName);

	m_uiChecksum = uiChecksum;
	m_iWidth = newImage.width();
	m_iHeight = newImage.height();

	if(m_eType == ATLASITEM_Image)
		m_rAlphaCrop = ImagePacker::crop(newImage);
	else // 'sub-atlases' should not be cropping their alpha because they rely on their own UV coordinates
		m_rAlphaCrop = QRect(0, 0, newImage.width(), newImage.height());

	// DO NOT clear 'm_iTextureIndex' as it's needed in the WidgetAtlasGroup::Repack()

	if(newImage.save(metaDir.path() % "/" % ConstructMetaFileName()) == false)
		HyGuiLog("Could not save frame image to meta directory: " % m_sName, LOGTYPE_Error);
}
//
//QDataStream &operator<<(QDataStream &out, AtlasFrame *const &rhs)
//{
//	out.writeRawData(reinterpret_cast<const char*>(&rhs), sizeof(rhs));
//	return out;
//}
//
//QDataStream &operator>>(QDataStream &in, AtlasFrame *rhs)
//{
//	in.readRawData(reinterpret_cast<char *>(rhs), sizeof(rhs));
//	return in;
//}
