/**************************************************************************
 *	IAssetItemData.h
 *
 *	Harmony Engine - Editor Tool
 *	Copyright (c) 2020 Jason Knobler
 *
 *	Harmony Editor Tool License:
 *	https://github.com/GameOverture/HarmonyEngine/blob/master/LICENSE
 *************************************************************************/
#ifndef IASSETITEMDATA_H
#define IASSETITEMDATA_H

#include "Global.h"
#include "TreeModelItemData.h"
#include "ProjectItemData.h"

#include <QUuid>

class IManagerModel;

class AssetItemData : public TreeModelItemData
{
	Q_OBJECT

protected:
	IManagerModel &						m_ModelRef;
	const QUuid							m_UNIQUE_ID;
	quint32								m_uiChecksum;
	quint32								m_uiBankId;
	QString								m_sMetaFileExt; // includes period

	QSet<ProjectItemData *>				m_DependencySet;
	uint								m_uiErrors;

public:
	AssetItemData(IManagerModel &modelRef, HyGuiItemType eType, QUuid uuid, quint32 uiChecksum, quint32 uiBankId, QString sName, QString sMetaFileExt, uint uiErrors);

	const QUuid &GetUuid();
	quint32 GetChecksum();
	quint32 GetBankId();
	void SetBankId(quint32 uiNewBankId);

	QString GetFilter();
	QString GetName() const;
	QString GetMetaFileExt() const;

	QSet<ProjectItemData *> GetLinks();
	void InsertLink(ProjectItemData *pProjItem);
	void RemoveLink(ProjectItemData *pProjItem);

	virtual QString ConstructMetaFileName();
	bool DeleteMetaFile();

	virtual QString GetPropertyInfo() = 0;

	void SetError(AssetErrorType eError);
	void ClearError(AssetErrorType eError);
	uint GetErrors();

	void GetJsonObj(QJsonObject &assetObj);

protected:
	virtual void InsertUniqueJson(QJsonObject &assetObj) = 0;
};

#endif // IASSETITEMDATA_H
