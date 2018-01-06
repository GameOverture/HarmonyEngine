/**************************************************************************
 *	WidgetOutputLog.cpp
 *
 *	Harmony Engine - Designer Tool
 *	Copyright (c) 2016 Jason Knobler
 *
 *	Harmony Designer Tool License:
 *	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
 *************************************************************************/
#include "WidgetOutputLog.h"
#include "ui_WidgetOutputLog.h"

#include "Global.h"

#include <QStringBuilder>
#include <QMessageBox>

WidgetOutputLog *WidgetOutputLog::sm_pInstance = NULL;

/*static*/ const QString WidgetOutputLog::sm_ksNormalHtml = "<font color=\"White\">";
/*static*/ const QString WidgetOutputLog::sm_ksWarningHtml = "<font color=\"Yellow\">";
/*static*/ const QString WidgetOutputLog::sm_ksErrorHtml = "<font color=\"DeepPink\">";
/*static*/ const QString WidgetOutputLog::sm_ksInfoHtml = "<font color=\"Aqua\">";
/*static*/ const QString WidgetOutputLog::sm_ksTitleHtml = "<font color=\"DarkCyan\">";
/*static*/ const QString WidgetOutputLog::sm_ksEndHtml = "</font>";

WidgetOutputLog::WidgetOutputLog(QWidget *parent) :
	QWidget(parent),
	ui(new Ui::WidgetOutputLog)
{
	Q_ASSERT(sm_pInstance == NULL);

	ui->setupUi(this);

	sm_pInstance = this;
}

WidgetOutputLog::~WidgetOutputLog()
{
	delete ui;
}

/*static*/ void WidgetOutputLog::Log(QString &sMessage, eLogType eType /*= LOGTYPE_Normal*/)
{
	QPlainTextEdit *pTxt = sm_pInstance->ui->m_TextLog;

	QString sLogMsg = sMessage;

	switch(eType)
	{
	case LOGTYPE_Normal:
		sLogMsg = sm_ksNormalHtml % sLogMsg;
		break;
	case LOGTYPE_Warning:
		QMessageBox::warning(sm_pInstance->parentWidget(), HyDesignerToolName, sLogMsg);
		sLogMsg = sm_ksWarningHtml % sLogMsg; 
		break;
	case LOGTYPE_Error:
		QMessageBox::critical(sm_pInstance->parentWidget(), HyDesignerToolName, sLogMsg);
		sLogMsg = sm_ksErrorHtml % sLogMsg;
		break;
	case LOGTYPE_Info:
		sLogMsg = sm_ksInfoHtml % sLogMsg;
		break;
	case LOGTYPE_Title:
		sLogMsg = "<br>" % sm_ksTitleHtml % "<b>----- " % sLogMsg % " -----</b><br>";
		break;
	}

	sLogMsg = sLogMsg % sm_ksEndHtml;

	pTxt->appendHtml(sLogMsg);
}
