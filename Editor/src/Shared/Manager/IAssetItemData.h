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
protected:
	IManagerModel &						m_ModelRef;
	const QUuid							m_UNIQUE_ID;
	quint32								m_uiChecksum;
	quint32								m_uiBankId;
	QString								m_sName;
	QString								m_sMetaFileExt; // includes period

	QSet<ProjectItemData *>				m_DependencySet;
	uint								m_uiErrors;

public:
	AssetItemData(IManagerModel &modelRef, QUuid uuid, quint32 uiChecksum, quint32 uiBankId, QString sName, QString sMetaFileExt, uint uiErrors);

	const QUuid &GetUuid();
	quint32 GetChecksum();
	quint32 GetBankId();
	void SetBankId(quint32 uiNewBankId);

	QString GetFilter() const;
	QString GetName() const;
	void SetName(QString sNewName);

	QSet<ProjectItemData *> GetLinks();
	void InsertLink(ProjectItemData *pProjItem);
	void RemoveLink(ProjectItemData *pProjItem);

	QString ConstructMetaFileName();
	bool DeleteMetaFile();

	void SetError(AtlasFrameError eError);
	void ClearError(AtlasFrameError eError);
	uint GetErrors();

	virtual void GetJsonObj(QJsonObject &frameObj) = 0;
};

#endif // IASSETITEMDATA_H
