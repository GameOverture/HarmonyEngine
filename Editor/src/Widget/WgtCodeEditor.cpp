/**************************************************************************
 *	WgtCodeEditor.cpp
 *
 *	Harmony Engine - Editor Tool
 *	Copyright (c) 2024 Jason Knobler
 *
 *	Harmony Editor Tool License:
 *	https://github.com/GameOverture/HarmonyEngine/blob/master/LICENSE
 *************************************************************************/
#include "Global.h"
#include "WgtCodeEditor.h"
#include "ui_WgtCodeEditor.h"
#include "SourceFile.h"
#include "MainWindow.h"

#include <QGLSLCompleter>
#include <QSyntaxStyle>
#include <QCXXHighlighter>
#include <QGLSLHighlighter>
#include <QJSONHighlighter>
#include <QMessageBox>

QMap<Theme, QSyntaxStyle *>	WgtCodeEditor::sm_ThemeStyleMap;

WgtCodeEditor::WgtCodeEditor(QWidget *pParent /*= nullptr*/) :
	QWidget(pParent),
	ui(new Ui::WgtCodeEditor)
{
	ui->setupUi(this);

	QObject::connect(&m_OpenFileWatcher, SIGNAL(fileChanged(const QString &)), this, SLOT(OnFileChanged(const QString &)));
}

/*virtual*/ WgtCodeEditor::~WgtCodeEditor()
{
	delete ui;
}

/*static*/ void WgtCodeEditor::InitThemeStyleMap()
{
	sm_ThemeStyleMap.insert(THEME_CorpyNT6, QSyntaxStyle::defaultStyle());
	sm_ThemeStyleMap.insert(THEME_Decemberween, new QSyntaxStyle());

	QString sPath = QString::fromUtf8(":/styles/Decemberween.xml");
	QFile internalFile(sPath);
	if(!internalFile.open(QIODevice::ReadOnly))
	{
		HyGuiLog("WgtCodeEditor::WgtCodeEditor failed to open theme file: " % sPath, LOGTYPE_Error);
		return;
	}
	bool bSuccess = sm_ThemeStyleMap[THEME_Decemberween]->load(internalFile.readAll());
	if(bSuccess == false)
		HyGuiLog("WgtCodeEditor::WgtCodeEditor failed to load theme", LOGTYPE_Error);
	internalFile.close();
}

/*static*/ void WgtCodeEditor::DestroyThemeStyleMap()
{
	for(auto iter = sm_ThemeStyleMap.begin(); iter != sm_ThemeStyleMap.end(); ++iter)
	{
		if(iter.value() != QSyntaxStyle::defaultStyle())
			delete iter.value();
	}
	sm_ThemeStyleMap.clear();
}

bool WgtCodeEditor::IsDirty(const SourceFile *pSrcFile) const
{
	for(const OpenSrcFile &openFileRef : m_OpenFileList)
	{
		if(openFileRef.m_pSourceFile == pSrcFile)
			return openFileRef.m_pCodeEditor->document()->isModified();
	}
	
	return false;
}

void WgtCodeEditor::SetReadOnly(bool bReadOnly)
{
	ui->chkReadOnly->setChecked(bReadOnly);
}

void WgtCodeEditor::SetTheme(Theme eTheme)
{
	if(sm_ThemeStyleMap.contains(eTheme) == false)
	{
		HyGuiLog("WgtCodeEditor::SetTheme was given unknown theme", LOGTYPE_Error);
		return;
	}

	for(const OpenSrcFile &openFileRef : m_OpenFileList)
		openFileRef.m_pCodeEditor->setSyntaxStyle(sm_ThemeStyleMap[eTheme]);
}

void WgtCodeEditor::Open(SourceFile *pSrcFile)
{
	if(pSrcFile == nullptr)
		return;
	for(OpenSrcFile &openFileRef : m_OpenFileList)
	{
		if(openFileRef.m_pSourceFile == pSrcFile)
			return;
	}

	// Create new TAB/OpenSrcFile
	m_OpenFileList.push_back(OpenSrcFile());
	OpenSrcFile &openFileRef = m_OpenFileList.back();
	openFileRef.m_pSourceFile = pSrcFile;
	
	openFileRef.m_pCodeEditor = new QCodeEditor();
	openFileRef.m_pCodeEditor->setTabStopWidth(openFileRef.m_pCodeEditor->fontMetrics().horizontalAdvance(' ') * 4);
	openFileRef.m_pCodeEditor->setTabReplace(false);
	openFileRef.m_pCodeEditor->setSyntaxStyle(sm_ThemeStyleMap[MainWindow::GetTheme()]);

	switch(openFileRef.m_pSourceFile->GetType())
	{
	case ITEM_Source:
	case ITEM_Header:
		openFileRef.m_pCodeEditor->setCompleter(nullptr);
		openFileRef.m_pCodeEditor->setHighlighter(new QCXXHighlighter());
		break;

	//case ITEM_GLSL:
	//	openFileRef.m_pCodeEditor->setCompleter(new QGLSLCompleter());
	//	openFileRef.m_pCodeEditor->setHighlighter(new QCXXHighlighter());
	//	break;

	default:
		HyGuiLog("WgtCodeEditor::SetSourceFile unknown file type: " % QString::number(openFileRef.m_pSourceFile->GetType()), LOGTYPE_Error);
		break;
	}

	openFileRef.m_pCodeEditor->setReadOnly(ui->chkReadOnly->isChecked());
	openFileRef.m_pCodeEditor->setWordWrapMode(ui->chkWordWrap->isChecked() ? QTextOption::WrapAtWordBoundaryOrAnywhere : QTextOption::NoWrap);
	openFileRef.m_pCodeEditor->setAutoParentheses(ui->chkAutoBracket->isChecked());
	openFileRef.m_pCodeEditor->setTabReplace(ui->chkTabsToSpaces->isChecked());
	openFileRef.m_pCodeEditor->setAutoIndentation(ui->chkAutoIndentation->isChecked());

	ui->tabWidget->addTab(openFileRef.m_pCodeEditor, openFileRef.m_pSourceFile->GetSourceIcon(), openFileRef.m_pSourceFile->GetName());
	m_OpenFileWatcher.addPath(openFileRef.m_pSourceFile->GetAbsMetaFilePath());

	openFileRef.m_pSourceFile->AddCodeEditor(this);

	Reload(openFileRef.m_pSourceFile);
}

void WgtCodeEditor::Reload(SourceFile *pSrcFile)
{
	if(pSrcFile == nullptr)
		return;

	for(OpenSrcFile &openFileRef : m_OpenFileList)
	{
		if(openFileRef.m_pSourceFile == pSrcFile)
		{
			QFile sourceFile(openFileRef.m_pSourceFile->GetAbsMetaFilePath());
			if(sourceFile.open(QIODevice::ReadOnly | QIODevice::Text))
			{
				openFileRef.m_pCodeEditor->setPlainText(sourceFile.readAll());
				sourceFile.close();

				openFileRef.m_pCodeEditor->document()->setModified(false);
			}
			else
				HyGuiLog("WgtCodeEditor::Reload failed to open file: " % openFileRef.m_pSourceFile->GetAbsMetaFilePath(), LOGTYPE_Debug);

			break;
		}
	}
}

bool WgtCodeEditor::Close(SourceFile *pSrcFile)
{
	for(OpenSrcFile &openFileRef : m_OpenFileList)
	{
		if(openFileRef.m_pSourceFile == pSrcFile)
		{
			if(IsDirty(openFileRef.m_pSourceFile))
			{
				int iMsgReturn = QMessageBox::question(this, "Unsaved Code File Changes", "Save changes to " % openFileRef.m_pSourceFile->GetName() % "?", QMessageBox::Yes | QMessageBox::No | QMessageBox::Cancel);
				if(iMsgReturn == QMessageBox::Yes)
					Save(openFileRef.m_pSourceFile);
				else if(iMsgReturn == QMessageBox::Cancel)
					return false;
			}

			openFileRef.m_pSourceFile->RemoveCodeEditor(this);
			m_OpenFileWatcher.removePath(openFileRef.m_pSourceFile->GetAbsMetaFilePath());
			ui->tabWidget->removeTab(ui->tabWidget->indexOf(openFileRef.m_pCodeEditor));
			delete openFileRef.m_pCodeEditor;
			
			m_OpenFileList.removeOne(openFileRef);
			break;
		}
	}

	return true;
}

void WgtCodeEditor::CloseUnmodified()
{
	for(int i = 0; i < m_OpenFileList.size();)
	{
		if(IsDirty(m_OpenFileList[i].m_pSourceFile) == false && Close(m_OpenFileList[i].m_pSourceFile))
			continue;
		++i;
	}
}

void WgtCodeEditor::Save(SourceFile *pSrcFile)
{
	if(pSrcFile == nullptr)
		return;

	for(OpenSrcFile &openFileRef : m_OpenFileList)
	{
		if(openFileRef.m_pSourceFile == pSrcFile)
		{
			QFile sourceFile(openFileRef.m_pSourceFile->GetAbsMetaFilePath());
			if(sourceFile.open(QIODevice::WriteOnly | QIODevice::Text))
			{
				sourceFile.write(openFileRef.m_pCodeEditor->toPlainText().toUtf8());
				sourceFile.close();

				openFileRef.m_pCodeEditor->document()->setModified(false);
			}
			else
				HyGuiLog("WgtCodeEditor::Save failed to open file: " % openFileRef.m_pSourceFile->GetAbsMetaFilePath(), LOGTYPE_Error);

			break;
		}
	}
}

void WgtCodeEditor::on_chkReadOnly_toggled(bool bChecked)
{
	for(OpenSrcFile &openFileRef : m_OpenFileList)
		openFileRef.m_pCodeEditor->setReadOnly(bChecked);

	ui->wgtCtrls->setVisible(!bChecked);
}

void WgtCodeEditor::on_chkWordWrap_toggled(bool bChecked)
{
	for(OpenSrcFile &openFileRef : m_OpenFileList)
		openFileRef.m_pCodeEditor->setWordWrapMode(bChecked ? QTextOption::WrapAtWordBoundaryOrAnywhere : QTextOption::NoWrap);
}

void WgtCodeEditor::on_chkAutoBracket_toggled(bool bChecked)
{
	for(OpenSrcFile &openFileRef : m_OpenFileList)
		openFileRef.m_pCodeEditor->setAutoParentheses(bChecked);
}

void WgtCodeEditor::on_chkTabsToSpaces_toggled(bool bChecked)
{
	for(OpenSrcFile &openFileRef : m_OpenFileList)
		openFileRef.m_pCodeEditor->setTabReplace(bChecked);
}

void WgtCodeEditor::on_chkAutoIndentation_toggled(bool bChecked)
{
	for(OpenSrcFile &openFileRef : m_OpenFileList)
		openFileRef.m_pCodeEditor->setAutoIndentation(bChecked);
}

void WgtCodeEditor::OnFileChanged(const QString &sFilePath)
{
	// Note: As a safety measure, many applications save an open file by writing a new file
	// and then deleting the old one. In your slot function, you can check m_FileWatcher.files().contains(path)
	// If it returns false, check whether the file still exists and then call addPath() to continue watching it.
	QFileInfo changedFileInfo(sFilePath);

	bool bFoundInOpenList = false;
	for(OpenSrcFile &openFileRef : m_OpenFileList)
	{
		QFileInfo openFileInfo(openFileRef.m_pSourceFile->GetAbsMetaFilePath());
		if(changedFileInfo == openFileInfo)
		{
			//int iDlgReturn = QMessageBox::question(nullptr, "File Changed", "The file " % changedFileInfo.fileName() % " has changed on disk. Do you want to reload it?", QMessageBox::Yes | QMessageBox::No, QMessageBox::Yes);
			//if(iDlgReturn == QMessageBox::Yes)
				Reload(openFileRef.m_pSourceFile);
			
			bFoundInOpenList = true;
			break;
		}
	}

	// If not found in m_OpenFileList, then it was renamed or removed externally. This is NOT proper usage, but try to handle it gracefully
	if(bFoundInOpenList == false)
		HyGuiLog("The file " % changedFileInfo.fileName() % " was renamed or removed externally. This file is not currently open in the editor, so it will not be reloaded.", LOGTYPE_Error);
}
