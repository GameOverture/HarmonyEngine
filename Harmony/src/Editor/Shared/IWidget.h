/**************************************************************************
*	IWidget.h
*
*	Harmony Engine - Editor Tool
*	Copyright (c) 2019 Jason Knobler
*
*	Harmony Editor Tool License:
*	https://github.com/GameOverture/HarmonyEngine/blob/master/LICENSE
*************************************************************************/
#ifndef IWIDGET_H
#define IWIDGET_H

#include <QWidget>
#include <QMenu>

namespace Ui {
class IWidget;
}

class IStateData;
class ProjectItem;

class IWidget : public QWidget
{
	Q_OBJECT

protected:
	ProjectItem &			m_ItemRef;

public:
	explicit IWidget(ProjectItem &itemRef, QWidget *pParent = nullptr);
	~IWidget();

	ProjectItem &GetItem();

	int GetCurStateIndex();
	IStateData *GetCurStateData();
	void UpdateActions();
	void FocusState(int iStateIndex, QVariant subState);

	virtual void OnGiveMenuActions(QMenu *pMenu) = 0;
	virtual void OnUpdateActions() = 0;
	virtual void OnFocusState(int iStateIndex, QVariant subState) = 0;

private Q_SLOTS:
	void on_cmbStates_currentIndexChanged(int index);

	void on_actionAddState_triggered();

	void on_actionRemoveState_triggered();

	void on_actionRenameState_triggered();

	void on_actionOrderStateBackwards_triggered();

	void on_actionOrderStateForwards_triggered();

private:
	Ui::IWidget *ui;
};

#endif // IWIDGET_H
