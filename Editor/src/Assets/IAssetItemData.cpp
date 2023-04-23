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

IAssetItemData::IAssetItemData(IManagerModel &modelRef, ItemType eType, QUuid uuid, quint32 uiChecksum, quint32 uiBankId, QString sName, QString sMetaFileExt, uint uiErrors) :
	TreeModelItemData(eType, uuid, sName),
	m_ModelRef(modelRef),
	m_uiChecksum(uiChecksum),
	m_uiBankId(uiBankId),
	m_sMetaFileExt(sMetaFileExt),
	m_uiErrors(uiErrors) // '0' when there is no error
{
	m_bIsAssetItem = true;
}

AssetManagerType IAssetItemData::GetManagerAssetType() const
{
	return m_ModelRef.GetAssetType();
}

quint32 IAssetItemData::GetChecksum()
{
	return m_uiChecksum;
}

quint32 IAssetItemData::GetBankId()
{
	return m_uiBankId;
}

void IAssetItemData::SetBankId(quint32 uiNewBankId)
{
	m_uiBankId = uiNewBankId;
}

Project &IAssetItemData::GetProject()
{
	return m_ModelRef.GetProjOwner();
}

QString IAssetItemData::GetFilter() const
{
	IAssetItemData *pAssetData = const_cast<IAssetItemData *>(this);
	return m_ModelRef.AssembleFilter(pAssetData, false);
}

QString IAssetItemData::GetName() const
{
	return m_sName;
}

/*virtual*/ QString IAssetItemData::ConstructMetaFileName() const
{
	QString sMetaName;
	sMetaName = sMetaName.asprintf("%010u", m_uiChecksum);
	sMetaName += m_sMetaFileExt;

	return sMetaName;
}

QString IAssetItemData::GetMetaFileExt() const
{
	return m_sMetaFileExt;
}

QString IAssetItemData::GetAbsMetaFilePath() const
{
	return m_ModelRef.GetMetaDir().absoluteFilePath(ConstructMetaFileName());
}

bool IAssetItemData::DeleteMetaFile()
{
	QFile imageFile(GetAbsMetaFilePath());
	if(imageFile.remove() == false)
		return false;

	return true;
}

void IAssetItemData::SetError(AssetErrorType eError)
{
	if(eError == ASSETERROR_CannotFindMetaFile)
		HyGuiLog(m_sName % " - Cannot find meta file", LOGTYPE_Error);

	m_uiErrors |= (1 << eError);

	//UpdateTreeItemIconAndToolTip();
}

void IAssetItemData::ClearError(AssetErrorType eError)
{
	m_uiErrors &= ~(1 << eError);

	//UpdateTreeItemIconAndToolTip();
}

uint IAssetItemData::GetErrors()
{
	return m_uiErrors;
}

void IAssetItemData::GetJsonObj(QJsonObject &assetObj)
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
