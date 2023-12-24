/**************************************************************************
 *	AuxOutputLog.cpp
 *
 *	Harmony Engine - Editor Tool
 *	Copyright (c) 2023 Jason Knobler
 *
 *	Harmony Editor Tool License:
 *	https://github.com/GameOverture/HarmonyEngine/blob/master/LICENSE
 *************************************************************************/
#include "Global.h"
#include "AuxOutputLog.h"
#include "ui_AuxOutputLog.h"

#include <QStringBuilder>
#include <QMessageBox>

AuxOutputLog *AuxOutputLog::sm_pInstance = nullptr;

/*static*/ const QString AuxOutputLog::sm_ksNormalHtml = "<font color=\"White\">";
/*static*/ const QString AuxOutputLog::sm_ksWarningHtml = "<font color=\"Yellow\">";
/*static*/ const QString AuxOutputLog::sm_ksErrorHtml = "<font color=\"DeepPink\">";
/*static*/ const QString AuxOutputLog::sm_ksInfoHtml = "<font color=\"Aqua\">";
/*static*/ const QString AuxOutputLog::sm_ksTitleHtml = "<font color=\"DarkCyan\">";
/*static*/ const QString AuxOutputLog::sm_ksEndHtml = "</font>";

AuxOutputLog::AuxOutputLog(QWidget *parent) :
	QWidget(parent),
	ui(new Ui::AuxOutputLog)
{
	Q_ASSERT(sm_pInstance == nullptr);

	ui->setupUi(this);

	sm_pInstance = this;
}

AuxOutputLog::~AuxOutputLog()
{
	delete ui;
}

/*static*/ void AuxOutputLog::Log(QString sMessage, eLogType eType /*= LOGTYPE_Normal*/)
{
	QPlainTextEdit *pTxt = sm_pInstance->ui->m_TextLog;

	QString sLogMsg = sMessage;

	switch(eType)
	{
	case LOGTYPE_Normal:
		sLogMsg = sm_ksNormalHtml % sLogMsg;
		break;
	case LOGTYPE_Warning:
		QMessageBox::warning(sm_pInstance->parentWidget(), HyEditorToolName, sLogMsg);
		sLogMsg = sm_ksWarningHtml % sLogMsg; 
		break;
	case LOGTYPE_Error:
		QMessageBox::critical(sm_pInstance->parentWidget(), HyEditorToolName, sLogMsg);
		sLogMsg = sm_ksErrorHtml % sLogMsg;
		break;
	case LOGTYPE_Info:
		sLogMsg = sm_ksInfoHtml % sLogMsg;
		break;
	case LOGTYPE_Title:
		sLogMsg = "<br>" % sm_ksTitleHtml % "<b>----- " % sLogMsg % " -----</b><br>";
		break;
	case LOGTYPE_Debug:
	default:
		return;
		break;
	}

	sLogMsg = sLogMsg % sm_ksEndHtml;

	pTxt->appendHtml(sLogMsg);
}
