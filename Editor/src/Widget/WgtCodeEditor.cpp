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

#include <QGLSLCompleter>
#include <QSyntaxStyle>
#include <QCXXHighlighter>
#include <QGLSLHighlighter>
#include <QJSONHighlighter>

WgtCodeEditor::WgtCodeEditor(QWidget *pParent /*= nullptr*/) :
	QWidget(pParent),
	ui(new Ui::WgtCodeEditor),
	m_pSourceFile(nullptr),
	m_pCompleter(nullptr),
	m_pSyntaxHighlighter(nullptr)
{
	ui->setupUi(this);

	int iSpaceWidth = ui->codeEditor->fontMetrics().horizontalAdvance(' ');
	ui->codeEditor->setTabStopWidth(iSpaceWidth * 4);
	ui->codeEditor->setTabReplace(false);

	m_ThemeStyleMap.insert(THEME_CorpyNT6, QSyntaxStyle::defaultStyle());
	m_ThemeStyleMap.insert(THEME_Decemberween, new QSyntaxStyle(this));

	QString sPath = QString::fromUtf8(":/styles/Decemberween.xml");
	QFile internalFile(sPath);
	if(!internalFile.open(QIODevice::ReadOnly))
	{
		HyGuiLog("WgtCodeEditor::WgtCodeEditor failed to open theme file: " % sPath, LOGTYPE_Error);
		return;
	}
	bool bSuccess = m_ThemeStyleMap[THEME_Decemberween]->load(internalFile.readAll());
	if(bSuccess == false)
		HyGuiLog("WgtCodeEditor::WgtCodeEditor failed to load theme", LOGTYPE_Error);
	internalFile.close();
}

/*virtual*/ WgtCodeEditor::~WgtCodeEditor()
{
	delete ui;
}

void WgtCodeEditor::SetTheme(Theme eTheme)
{
	if(m_ThemeStyleMap.contains(eTheme) == false)
	{
		HyGuiLog("WgtCodeEditor::SetTheme was given unknown theme", LOGTYPE_Error);
		return;
	}

	ui->codeEditor->setSyntaxStyle(m_ThemeStyleMap[eTheme]);
}

void WgtCodeEditor::SetSourceFile(SourceFile *pSourceFileRef)
{
	if(pSourceFileRef == nullptr)
	{
		ui->codeEditor->setPlainText("");
		m_pSourceFile = nullptr;
		return;
	}

	m_pSourceFile = pSourceFileRef;

	ui->codeEditor->setCompleter(nullptr);
	ui->codeEditor->setHighlighter(nullptr);
	delete m_pCompleter;
	delete m_pSyntaxHighlighter;

	switch(m_pSourceFile->GetType())
	{
	case ITEM_Source:
	case ITEM_Header:
		m_pCompleter = nullptr;
		m_pSyntaxHighlighter = new QCXXHighlighter();
		break;

	//case ITEM_GLSL:
	//	m_pCompleter = new QGLSLCompleter(this);
	//	break;

	default:
		HyGuiLog("WgtCodeEditor::SetSourceFile unknown file type: " % QString::number(m_pSourceFile->GetType()), LOGTYPE_Error);
		break;
	}

	ui->codeEditor->setCompleter(m_pCompleter);
	ui->codeEditor->setHighlighter(m_pSyntaxHighlighter);

	Reload();
}

void WgtCodeEditor::Reload()
{
	if(m_pSourceFile == nullptr)
		return;

	QFile sourceFile(m_pSourceFile->GetAbsMetaFilePath());
	if(sourceFile.open(QIODevice::ReadOnly | QIODevice::Text))
	{
		ui->codeEditor->setPlainText(sourceFile.readAll());
		sourceFile.close();
	}
	else
		HyGuiLog("WgtCodeEditor::Reload failed to open file: " % m_pSourceFile->GetAbsMetaFilePath(), LOGTYPE_Error);
}
