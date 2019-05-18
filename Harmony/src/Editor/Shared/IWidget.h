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
	void OnCurrentIndexChanged(int index);

	void OnAddStateTriggered();

	void OnRemoveStateTriggered();

	void OnRenameStateTriggered();

	void OnOrderStateBackwardsTriggered();

	void OnOrderStateForwardsTriggered();

private:
	Ui::IWidget *ui;
};

#endif // IWIDGET_H
