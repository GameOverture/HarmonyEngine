/**************************************************************************
 *	WgtCodeEditor.h
 *
 *	Harmony Engine - Editor Tool
 *	Copyright (c) 2024 Jason Knobler
 *
 *	Harmony Editor Tool License:
 *	https://github.com/GameOverture/HarmonyEngine/blob/master/LICENSE
 *************************************************************************/
#ifndef WgtCodeEditor_H
#define WgtCodeEditor_H

#include "Global.h"
#include "vendor/QCodeEditor/include/internal/QCodeEditor.hpp"

#include <QWidget>
#include <QFileSystemWatcher>

class QSyntaxStyle;
class SourceFile;
class QCompleter;
class QStyleSyntaxHighlighter;

namespace Ui {
class WgtCodeEditor;
}

class SourceSettingsDlg;

class WgtCodeEditor : public QWidget
{
	Q_OBJECT

	static QMap<Theme, QSyntaxStyle *>		sm_ThemeStyleMap;
	
	struct OpenSrcFile
	{
		SourceFile *						m_pSourceFile;
		QCodeEditor *						m_pCodeEditor;
		
		OpenSrcFile() :
			m_pSourceFile(nullptr),
			m_pCodeEditor(nullptr)
		{ }

		bool operator==(const OpenSrcFile &rhs) const
		{
			return m_pSourceFile == rhs.m_pSourceFile && m_pCodeEditor == rhs.m_pCodeEditor;
		}
	};
	QList<OpenSrcFile>						m_OpenFileList;
	QFileSystemWatcher						m_OpenFileWatcher;
	
public:
	explicit WgtCodeEditor(QWidget *pParent = nullptr);
	virtual ~WgtCodeEditor();

	static void InitThemeStyleMap();
	static void DestroyThemeStyleMap();

	bool IsDirty(const SourceFile *pSrcFile) const;
	void SetReadOnly(bool bReadOnly);
	void SetTheme(Theme eTheme);

	void Open(SourceFile *pSrcFile);
	void Reload(SourceFile *pSrcFile);
	bool Close(SourceFile *pSrcFile);
	void CloseUnmodified();

	void Save(SourceFile *pSrcFile);
	
private Q_SLOTS:
	void on_chkReadOnly_toggled(bool bChecked);
	void on_chkWordWrap_toggled(bool bChecked);
	void on_chkAutoBracket_toggled(bool bChecked);
	void on_chkTabsToSpaces_toggled(bool bChecked);
	void on_chkAutoIndentation_toggled(bool bChecked);
	void OnFileChanged(const QString &sFilePath);

private:
	Ui::WgtCodeEditor *ui;

Q_SIGNALS:
	void OnDirty();
};

#endif // WgtCodeEditor_H
