/**************************************************************************
 *	DlgTabCycle.h
 *
 *	Harmony Engine - HyEditor
 *	Copyright (c) 2024 Jason Knobler
 *
 *	GPL/LGPL License:
 *	https://github.com/GameOverture/HarmonyEngine/blob/main/Editor/LICENSE
 *************************************************************************/
#ifndef DLGTABCYCLE_H
#define DLGTABCYCLE_H

#include "ExplorerItemData.h"
#include "Project.h"

#include <QDialog>
#include <QListWidgetItem>

namespace Ui {
class DlgTabCycle;
}

class DlgTabCycle : public QDialog
{
	Q_OBJECT

	Project *			m_pProjectOwner;

public:
	explicit DlgTabCycle(ProjectTabBar *pProjTabBar, bool bSelectNext, QWidget *pParent = nullptr);
	~DlgTabCycle();

protected:
	virtual void keyPressEvent(QKeyEvent *pEvent) override;
	virtual void keyReleaseEvent(QKeyEvent *pEvent) override;

	virtual void closeEvent(QCloseEvent *pEvent) override;

private Q_SLOTS:
	void on_itemList_itemClicked(QListWidgetItem *pListWidgetItem);
	
private:
	Ui::DlgTabCycle *ui;
};

#endif // DLGTABCYCLE_H
