/**************************************************************************
 *	WgtAppendContainer.h
 *
 *	Harmony Engine - HyEditor
 *	Copyright (c) 2025 Jason Knobler
 *
 *	GPL/LGPL License:
 *	https://github.com/GameOverture/HarmonyEngine/blob/main/Editor/LICENSE
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
