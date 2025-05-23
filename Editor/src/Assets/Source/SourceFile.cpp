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
#include "WgtCodeEditor.h"

SourceFile::SourceFile(IManagerModel &modelRef,
					   QUuid uuid,
					   QString sBaseClass,
					   quint32 uiChecksum,
					   QString fileName,
					   uint uiErrors) :
	IAssetItemData(modelRef,
		QFileInfo(fileName).suffix().compare("cpp", Qt::CaseInsensitive) == 0 ? ITEM_Source : ITEM_Header,
		uuid, uiChecksum, 0, fileName, "." + QFileInfo(fileName).suffix(), uiErrors),
	m_sBaseClass(sBaseClass)
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

/*virtual*/ QString SourceFile::OnReplaceAllowed() /*override*/
{
	return QString();
}

/*virtual*/ void SourceFile::InsertUniqueJson(QJsonObject &frameObj) /*override*/
{
	frameObj.insert("baseClass", QJsonValue(m_sBaseClass));
}

void SourceFile::AddCodeEditor(WgtCodeEditor *pCodeEditor)
{
	m_CodeEditorSet.insert(pCodeEditor);
}

void SourceFile::RemoveCodeEditor(WgtCodeEditor *pCodeEditor)
{
	m_CodeEditorSet.remove(pCodeEditor);
}

bool SourceFile::TryCloseAllCodeEditors() // Returns false after the first fail
{
	while(m_CodeEditorSet.isEmpty() == false)
	{
		WgtCodeEditor *pCodeEditor = (*m_CodeEditorSet.begin());
		if(pCodeEditor->Close(this))
			RemoveCodeEditor(pCodeEditor);
		else
			return false;
	}

	return true;
}

QIcon SourceFile::GetSourceIcon() const
{
	SubIcon eSubIcon = SUBICON_None;
	for(WgtCodeEditor *pCodeEditor : m_CodeEditorSet)
	{
		if(pCodeEditor && pCodeEditor->IsDirty(this))
		{
			eSubIcon = SUBICON_Dirty;
			break;
		}
	}

	return GetIcon(eSubIcon);
}

void SourceFile::UpdateChecksum(quint32 uiChecksum)
{
	m_uiChecksum = uiChecksum;
}

/*virtual*/ void SourceFile::SetText(QString sText) /*override*/
{
	QFile srcFile(m_ModelRef.GetMetaDir().absoluteFilePath(ConstructMetaFileName()));
	
	IAssetItemData::SetText(sText);
	srcFile.rename(m_ModelRef.GetMetaDir().absoluteFilePath(ConstructMetaFileName()));
}
