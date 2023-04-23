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

class SourceFile : public IAssetItemData
{
	Q_OBJECT

public:
	SourceFile(IManagerModel &modelRef,
			   QUuid uuid,
			   quint32 uiChecksum,			// Checksum is the (filter path + filename) string all lowercase with forward slashes ('/'). No filter will not have any forward slashes.
			   QString fileName,			// Includes file extension
			   uint uiErrors);
	~SourceFile();

	virtual QString ConstructMetaFileName() const override;
	virtual QString GetPropertyInfo() override;
	virtual void InsertUniqueJson(QJsonObject &frameObj) override;

	void UpdateChecksum(quint32 uiChecksum);
};

#endif // SourceFile_H
