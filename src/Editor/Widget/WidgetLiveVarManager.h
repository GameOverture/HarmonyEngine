/**************************************************************************
 *	WidgetLiveVarManager.h
 *
 *	Harmony Engine - Editor Tool
 *	Copyright (c) 2016 Jason Knobler
 *
 *	Harmony Editor Tool License:
 *	https://github.com/GameOverture/HarmonyEngine/blob/master/LICENSE
 *************************************************************************/
#ifndef WIDGETLIVEVARMANAGER_H
#define WIDGETLIVEVARMANAGER_H

#include <QWidget>

namespace Ui {
class WidgetLiveVarManager;
}

class WidgetLiveVarManager : public QWidget
{
	Q_OBJECT
	
	enum eVarType
	{
		LIVEVAR_Int = 0,
		LIVEVAR_Float,
		LIVEVAR_Boolean
	};
	
public:
	explicit WidgetLiveVarManager(QWidget *parent = 0);
	~WidgetLiveVarManager();
	
	void AddTabPage(QString sName, uint uiGuid);
	void AddVar(uint uiTabPageGuid, QString sGroup, QString sVarName, eVarType eType);
	
private:
	Ui::WidgetLiveVarManager *ui;
};

#endif // WIDGETLIVEVARMANAGER_H
