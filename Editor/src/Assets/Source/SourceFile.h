/**************************************************************************
 *	SourceFile.h
 *
 *	Harmony Engine - Editor Tool
 *	Copyright (c) 2016 Jason Knobler
 *
 *	Harmony Editor Tool License:
 *	https://github.com/GameOverture/HarmonyEngine/blob/master/LICENSE
 *************************************************************************/
#ifndef SourceFile_H
#define SourceFile_H

#include "IAssetItemData.h"
#include "ProjectItemData.h"

#include <QWidget>
#include <QSet>
#include <QJsonObject>
#include <QDataStream>

class SourceFile : public AssetItemData
{
	Q_OBJECT

public:
	SourceFile(IManagerModel &modelRef,
			   QUuid uuid,
			   quint32 uiChecksum,			// Checksum is the (filter path + filename) string all lowercase (with forward slashes / )
			   QString fileName,			// Includes file extension
			   uint uiErrors);
	~SourceFile();

	virtual QString ConstructMetaFileName() override;
	virtual QString GetPropertyInfo() override;
	virtual void InsertUniqueJson(QJsonObject &frameObj) override;

};

#endif // SourceFile_H
