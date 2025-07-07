/**************************************************************************
 *	WgtAppendContainer.h
 *
 *	Harmony Engine - Editor Tool
 *	Copyright (c) 2025 Jason Knobler
 *
 *	Harmony Editor Tool License:
 *	https://github.com/GameOverture/HarmonyEngine/blob/master/LICENSE
 *************************************************************************/
#ifndef WgtAppendContainer_H
#define WgtAppendContainer_H

#include "Global.h"
#include "Project.h"

#include <QWidget>

namespace Ui {
class WgtAppendContainer;
}

class IWgtAppendWidget : public QWidget
{
	Q_OBJECT

public:
	virtual ~IWgtAppendWidget() {}
};

class WgtAppendContainer : public QWidget
{
	Q_OBJECT

	Ui::WgtAppendContainer *		ui;

	QList<IWgtAppendWidget *>		m_WidgetList;

public:
	WgtAppendContainer(QWidget *pParent = nullptr);
	virtual ~WgtAppendContainer();
	
	void RemoveWidget(IWgtAppendWidget *pRemoved);

	void Refresh();

protected:
	virtual IWgtAppendWidget *AllocWidget() = 0;

private Q_SLOTS:
	void on_actionAddNewElement_triggered();
};

#endif // WgtAppendContainer_H
