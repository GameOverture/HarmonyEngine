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
class Project;

class IAssetItemData : public TreeModelItemData
{
	Q_OBJECT

protected:
	IManagerModel &						m_ModelRef;
	quint32								m_uiChecksum;
	quint32								m_uiBankId;
	QString								m_sMetaFileExt; // includes period

	uint								m_uiErrors;

public:
	IAssetItemData(IManagerModel &modelRef, ItemType eType, QUuid uuid, quint32 uiChecksum, quint32 uiBankId, QString sName, QString sMetaFileExt, uint uiErrors);

	AssetManagerType GetManagerAssetType() const;
	quint32 GetChecksum();
	quint32 GetBankId();
	void SetBankId(quint32 uiNewBankId);

	Project &GetProject();

	QString GetFilter() const;
	QString GetName() const;

	virtual QString ConstructMetaFileName() const;
	QString GetMetaFileExt() const;
	QString GetAbsMetaFilePath() const;
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
