/**************************************************************************
 *	WidgetOutputLog.h
 *
 *	Harmony Engine - Designer Tool
 *	Copyright (c) 2016 Jason Knobler
 *
 *	Harmony Designer Tool License:
 *	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
 *************************************************************************/
#ifndef WIDGETOUTPUTLOG_H
#define WIDGETOUTPUTLOG_H

#include <QWidget>

namespace Ui {
class WidgetOutputLog;
}

enum eLogType
{
	LOGTYPE_Normal = 0,
	LOGTYPE_Warning,
	LOGTYPE_Error,
	LOGTYPE_Info,
	LOGTYPE_Title
};

class WidgetOutputLog : public QWidget
{
	Q_OBJECT

	static WidgetOutputLog *      sm_pInstance;

	static const QString    sm_ksNormalHtml;
	static const QString    sm_ksWarningHtml;
	static const QString    sm_ksErrorHtml;
	static const QString    sm_ksInfoHtml;
	static const QString    sm_ksTitleHtml;
	static const QString    sm_ksEndHtml;

public:
	explicit WidgetOutputLog(QWidget *parent = 0);
	~WidgetOutputLog();

	static void Log(QString &sMessage, eLogType eType = LOGTYPE_Normal);

private:
	Ui::WidgetOutputLog *ui;
};

#endif // WIDGETOUTPUTLOG_H
