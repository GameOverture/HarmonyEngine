/**************************************************************************
 *	AuxOutputLog.h
 *
 *	Harmony Engine - Editor Tool
 *	Copyright (c) 2023 Jason Knobler
 *
 *	Harmony Editor Tool License:
 *	https://github.com/GameOverture/HarmonyEngine/blob/master/LICENSE
 *************************************************************************/
#ifndef AUXOUTPUTLOG_H
#define AUXOUTPUTLOG_H

#include <QWidget>

namespace Ui {
class AuxOutputLog;
}

enum eLogType
{
	LOGTYPE_Normal = 0,
	LOGTYPE_Warning,
	LOGTYPE_Error,
	LOGTYPE_Info,
	LOGTYPE_Title
};

class AuxOutputLog : public QWidget
{
	Q_OBJECT

	static AuxOutputLog *		sm_pInstance;

	static const QString		sm_ksNormalHtml;
	static const QString		sm_ksWarningHtml;
	static const QString		sm_ksErrorHtml;
	static const QString		sm_ksInfoHtml;
	static const QString		sm_ksTitleHtml;
	static const QString		sm_ksEndHtml;

public:
	explicit AuxOutputLog(QWidget *parent = 0);
	/*virtual*/ ~AuxOutputLog();

	static void Log(QString sMessage, eLogType eType = LOGTYPE_Normal);

private:
	Ui::AuxOutputLog *ui;
};

#endif // AUXOUTPUTLOG_H
