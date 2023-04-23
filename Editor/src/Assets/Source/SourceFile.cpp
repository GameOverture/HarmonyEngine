/**************************************************************************
 *	SourceFile.cpp
 *
 *	Harmony Engine - Editor Tool
 *	Copyright (c) 2016 Jason Knobler
 *
 *	Harmony Editor Tool License:
 *	https://github.com/GameOverture/HarmonyEngine/blob/master/LICENSE
 *************************************************************************/
#include "Global.h"
#include "SourceFile.h"
#include "IManagerModel.h"
#include "_Dependencies/scriptum/imagepacker.h"

SourceFile::SourceFile(IManagerModel &modelRef,
					   QUuid uuid,
					   quint32 uiChecksum,
					   QString fileName,
					   uint uiErrors) :
	IAssetItemData(modelRef,
		QFileInfo(fileName).suffix().compare("cpp", Qt::CaseInsensitive) == 0 ? ITEM_Source : ITEM_Header,
		uuid, uiChecksum, 0, fileName, QFileInfo(fileName).suffix(), uiErrors)
{
}

SourceFile::~SourceFile()
{
}

/*virtual*/ QString SourceFile::ConstructMetaFileName() const /*override*/
{
	QString sCombinedPath;
	QString sFilterPath = GetFilter();
	if(sFilterPath.isEmpty())
		sCombinedPath = GetName();
	else
		sCombinedPath = sFilterPath + "/" + GetName();

	return sCombinedPath;
}

/*virtual*/ QString SourceFile::GetPropertyInfo() /*override*/
{
	return QString();
}

/*virtual*/ void SourceFile::InsertUniqueJson(QJsonObject &frameObj) /*override*/
{
	//frameObj.insert("width", QJsonValue(GetSize().width()));
	//frameObj.insert("height", QJsonValue(GetSize().height()));
	//frameObj.insert("textureIndex", QJsonValue(GetTextureIndex()));
	//frameObj.insert("x", QJsonValue(GetX()));
	//frameObj.insert("y", QJsonValue(GetY()));
	//frameObj.insert("cropLeft", QJsonValue(GetCrop().left()));
	//frameObj.insert("cropTop", QJsonValue(GetCrop().top()));
	//frameObj.insert("cropRight", QJsonValue(GetCrop().right()));
	//frameObj.insert("cropBottom", QJsonValue(GetCrop().bottom()));
	//frameObj.insert("textureFormat", HyAssets::GetTextureFormatName(m_eFormat).c_str());
	//frameObj.insert("textureFiltering", HyAssets::GetTextureFilteringName(m_eFiltering).c_str());
}

void SourceFile::UpdateChecksum(quint32 uiChecksum)
{
	m_uiChecksum = uiChecksum;
}
