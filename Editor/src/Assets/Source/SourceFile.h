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

class WgtCodeEditor;

class SourceFile : public IAssetItemData
{
	Q_OBJECT

	WgtCodeEditor *			m_pCodeEditor;	// When not nullptr, this editor has this file opened

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

	QIcon GetSourceIcon() const;

	void UpdateChecksum(quint32 uiChecksum);

	virtual void SetText(QString sText) override; // NOTE: This is just renaming the asset. It also renames the file on disk.
};

#endif // SourceFile_H
