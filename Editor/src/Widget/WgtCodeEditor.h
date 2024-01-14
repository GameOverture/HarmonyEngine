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
#include <QWidget>

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

	QMap<Theme, QSyntaxStyle *>				m_ThemeStyleMap;

	SourceFile *							m_pSourceFile;
	QCompleter *							m_pCompleter;
	QStyleSyntaxHighlighter *				m_pSyntaxHighlighter;
	
public:
	explicit WgtCodeEditor(QWidget *pParent = nullptr);
	virtual ~WgtCodeEditor();

	void SetTheme(Theme eTheme);

	void SetSourceFile(SourceFile *pSourceFileRef);
	void Reload();
	
private Q_SLOTS:

private:
	Ui::WgtCodeEditor *ui;

Q_SIGNALS:
	void OnDirty();
};

#endif // WgtCodeEditor_H
