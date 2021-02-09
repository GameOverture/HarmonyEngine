/**************************************************************************
 *	IAssetItemData.cpp
 *
 *	Harmony Engine - Editor Tool
 *	Copyright (c) 2020 Jason Knobler
 *
 *	Harmony Editor Tool License:
 *	https://github.com/GameOverture/HarmonyEngine/blob/master/LICENSE
 *************************************************************************/
#include "Global.h"
#include "IAssetItemData.h"
#include "IManagerModel.h"

AssetItemData::AssetItemData(IManagerModel &modelRef, HyGuiItemType eType, QUuid uuid, quint32 uiChecksum, quint32 uiBankId, QString sName, QString sMetaFileExt, uint uiErrors) :
	TreeModelItemData(eType, sName),
	m_ModelRef(modelRef),
	m_UNIQUE_ID(uuid),
	m_uiChecksum(uiChecksum),
	m_uiBankId(uiBankId),
	m_sMetaFileExt(sMetaFileExt),
	m_uiErrors(uiErrors) // '0' when there is no error
{
}

const QUuid &AssetItemData::GetUuid()
{
	return m_UNIQUE_ID;
}

quint32 AssetItemData::GetChecksum()
{
	return m_uiChecksum;
}

quint32 AssetItemData::GetBankId()
{
	return m_uiBankId;
}

void AssetItemData::SetBankId(quint32 uiNewBankId)
{
	m_uiBankId = uiNewBankId;
}

QString AssetItemData::GetFilter()
{
	return m_ModelRef.AssembleFilter(this, false);
}

QString AssetItemData::GetName() const
{
	return m_sName;
}

QString AssetItemData::GetMetaFileExt() const
{
	return m_sMetaFileExt;
}

QSet<ProjectItemData *> AssetItemData::GetLinks()
{
	return m_DependencySet;
}

void AssetItemData::InsertLink(ProjectItemData *pProjItem)
{
	m_DependencySet.insert(pProjItem);
}

void AssetItemData::RemoveLink(ProjectItemData *pProjItem)
{
	m_DependencySet.remove(pProjItem);
}

/*virtual*/ QString AssetItemData::ConstructMetaFileName()
{
	QString sMetaName;
	sMetaName = sMetaName.asprintf("%010u", m_uiChecksum);
	sMetaName += m_sMetaFileExt;

	return sMetaName;
}

bool AssetItemData::DeleteMetaFile()
{
	QFile imageFile(m_ModelRef.GetMetaDir().absoluteFilePath(ConstructMetaFileName()));
	if(imageFile.remove() == false)
		return false;

	return true;
}

void AssetItemData::SetError(AssetErrorType eError)
{
	if(eError == ASSETERROR_CannotFindMetaFile)
		HyGuiLog(m_sName % " - Cannot find meta file", LOGTYPE_Error);

	m_uiErrors |= (1 << eError);

	//UpdateTreeItemIconAndToolTip();
}

void AssetItemData::ClearError(AssetErrorType eError)
{
	m_uiErrors &= ~(1 << eError);

	//UpdateTreeItemIconAndToolTip();
}

uint AssetItemData::GetErrors()
{
	return m_uiErrors;
}

void AssetItemData::GetJsonObj(QJsonObject &assetObj)
{
	assetObj = QJsonObject();
	assetObj.insert("assetUUID", GetUuid().toString(QUuid::WithoutBraces));
	assetObj.insert("bankId", QJsonValue(static_cast<qint64>(GetBankId())));
	assetObj.insert("checksum", QJsonValue(static_cast<qint64>(GetChecksum())));
	assetObj.insert("name", QJsonValue(GetName()));
	assetObj.insert("filter", QJsonValue(m_ModelRef.AssembleFilter(this, false)));
	assetObj.insert("errors", QJsonValue(static_cast<int>(GetErrors())));
	assetObj.insert("itemType", QJsonValue(HyGlobal::ItemName(GetType(), false)));

	InsertUniqueJson(assetObj);
}
