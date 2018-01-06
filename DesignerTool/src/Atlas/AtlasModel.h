/**************************************************************************
 *	AtlasModel.h
 *
 *	Harmony Engine - Designer Tool
 *	Copyright (c) 2017 Jason Knobler
 *
 *	Harmony Designer Tool License:
 *	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
 *************************************************************************/
#ifndef ATLASMODEL_H
#define ATLASMODEL_H

#include <QAbstractListModel>

#include "AtlasFrame.h"
#include "_Dependencies/scriptum/imagepacker.h"

class AtlasModel : public QAbstractListModel
{
	Q_OBJECT

	Project *                                       m_pProjOwner;

	quint32                                         m_uiNextFrameId;
	quint32                                         m_uiNextAtlasId;

	QDir                                            m_MetaDir;
	QDir                                            m_RootDataDir;

	struct AtlasGrp
	{
		QDir                                        m_DataDir;
		
		QJsonObject                                 m_PackerSettings;
		ImagePacker                                 m_Packer;
		QList<AtlasFrame *>                         m_FrameList;
		
		AtlasGrp(QString sAbsDataDirPath) : m_DataDir(sAbsDataDirPath)
		{ }
		
		quint32 GetId()
		{
			return JSONOBJ_TOINT(m_PackerSettings, "atlasGrpId");
		}
		
		void SetPackerSettings()
		{
			m_Packer.sortOrder = m_PackerSettings["cmbSortOrder"].toInt();// m_iSortOrderIndex;//ui->cmbSortOrder->currentIndex();
			m_Packer.border.t = m_PackerSettings["sbFrameMarginTop"].toInt();// m_iFrameMarginTop;//ui->sbFrameMarginTop->value();
			m_Packer.border.l = m_PackerSettings["sbFrameMarginLeft"].toInt();// m_iFrameMarginLeft;//ui->sbFrameMarginLeft->value();
			m_Packer.border.r = m_PackerSettings["sbFrameMarginRight"].toInt();// m_iFrameMarginRight;//ui->sbFrameMarginRight->value();
			m_Packer.border.b = m_PackerSettings["sbFrameMarginBottom"].toInt();// m_iFrameMarginBottom;//ui->sbFrameMarginBottom->value();
			m_Packer.extrude = m_PackerSettings["extrude"].toInt();// m_iExtrude;//ui->extrude->value();
			m_Packer.merge = m_PackerSettings["chkMerge"].toBool();// m_bMerge;//ui->chkMerge->isChecked();
			m_Packer.square = m_PackerSettings["chkSquare"].toBool();// m_bSquare;//ui->chkSquare->isChecked();
			m_Packer.autosize = m_PackerSettings["chkAutosize"].toBool();// m_bAutoSize;//ui->chkAutosize->isChecked();
			m_Packer.minFillRate = m_PackerSettings["minFillRate"].toInt();// m_iFillRate;//ui->minFillRate->value();
			m_Packer.mergeBF = false;
			m_Packer.rotate = ImagePacker::NEVER;
		}
	};
	QList<AtlasGrp *>                               m_AtlasGrpList;

	QList<AtlasTreeItem *>                          m_TopLevelTreeItemList;
	
	class FrameLookup
	{
		QMap<quint32, AtlasFrame *>                 m_FrameIdMap;
		QMap<quint32, QList<AtlasFrame *> >         m_FrameChecksumMap;
		
	public:
		void AddLookup(AtlasFrame *pFrame);
		bool RemoveLookup(AtlasFrame *pFrame);  // Returns true if no remaining duplicates exist
		AtlasFrame *Find(quint32 uiId);
		bool DoesImageExist(quint32 uiChecksum);
	};
	FrameLookup                                     m_FrameLookup;

public:
	AtlasModel(Project *pProjOwner);
	virtual ~AtlasModel();
	
	Project *GetProjOwner();

	int GetNumAtlasGroups();
	QString GetAtlasGroupName(uint uiAtlasGrpIndex);
	
	QList<AtlasFrame *> GetFrames(uint uiAtlasGrpIndex);

	QJsonObject GetPackerSettings(uint uiAtlasGrpIndex);
	void SetPackerSettings(uint uiAtlasGrpIndex, QJsonObject newPackerSettingsObj);

	void StashTreeWidgets(QList<AtlasTreeItem *> treeItemList);
	QList<AtlasTreeItem *> GetTopLevelTreeItemList();

	QSize GetAtlasDimensions(uint uiAtlasGrpIndex);
	HyTextureFormat GetAtlasTextureType(uint uiAtlasGrpIndex);

	void WriteMetaSettings();
	void WriteMetaSettings(QJsonArray frameArray);

	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	AtlasFrame *CreateFrame(quint32 uiId, quint32 uiCRC, quint32 uiAtlasGrpId, QString sN, QRect rAlphaCrop, AtlasItemType eFrameType, int iW, int iH, int iX, int iY, int iTextureIndex, uint uiErrors);
	void RemoveFrame(AtlasFrame *pFrame);
	
	bool TransferFrame(AtlasFrame *pFrame, quint32 uiNewAtlasGrpId);

	AtlasFrame *GenerateFrame(ProjectItem *pItem, QString sName, QImage &newImage, quint32 uiAtlasGrpIndex, HyGuiItemType eType);
	void ReplaceFrame(AtlasFrame *pFrame, QString sName, QImage &newImage, bool bDoAtlasGroupRepack);

	QList<AtlasFrame *> RequestFrames(ProjectItem *pItem);
	QList<AtlasFrame *> RequestFrames(ProjectItem *pItem, QList<AtlasFrame *> requestList);
	QList<AtlasFrame *> RequestFramesById(ProjectItem *pItem, QList<quint32> requestList);

	void RelinquishFrames(ProjectItem *pItem, QList<AtlasFrame *> relinquishList);
	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	QSet<AtlasFrame *> ImportImages(QStringList sImportImgList, quint32 uiAtlasGrpId, HyGuiItemType eType, QList<AtlasTreeItem *> correspondingParentList);
	AtlasFrame *ImportImage(QString sName, QImage &newImage, quint32 uiAtlasGrpId, HyGuiItemType eType, AtlasTreeItem *pParent);

	AtlasTreeItem *CreateFilter(QString sName, AtlasTreeItem *pParent);

	bool DoesImageExist(quint32 uiChecksum);

	void SaveData();
	
	void CreateNewAtlasGrp(QString sName);
	void RemoveAtlasGrp(quint32 uiAtlasGrpId);

	uint GetAtlasGrpIndexFromAtlasGrpId(quint32 uiAtlasGrpId);
	quint32 GetAtlasGrpIdFromAtlasGrpIndex(uint uiAtlasGrpIndex);
	
	QFileInfoList GetExistingTextureInfoList(uint uiAtlasGrpIndex);

	void RepackAll(uint uiAtlasGrpIndex);
	void Repack(uint uiAtlasGrpIndex, QSet<int> repackTexIndicesSet, QSet<AtlasFrame *> newFramesSet);
	void ConstructAtlasTexture(uint uiAtlasGrpIndex, int iPackerBinIndex, int iActualTextureIndex);
	void Refresh();
	
	virtual int rowCount(const QModelIndex &parent = QModelIndex()) const;
	virtual QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;
	virtual bool setData(const QModelIndex &index, const QVariant &value, int role = Qt::EditRole);
	virtual QVariant headerData(int iIndex, Qt::Orientation orientation, int role = Qt::DisplayRole) const;

private Q_SLOTS:
	void OnRepackFinished();
};

#endif // ATLASMODEL_H
