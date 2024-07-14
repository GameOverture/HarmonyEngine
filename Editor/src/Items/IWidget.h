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
#include <QVBoxLayout>

namespace Ui {
class IWidget;
}

class IStateData;
class ProjectItemData;

class IWidget : public QWidget
{
	Q_OBJECT

protected:
	ProjectItemData &			m_ItemRef;

public:
	explicit IWidget(ProjectItemData &itemRef, QWidget *pParent = nullptr);
	~IWidget();

	ProjectItemData &GetItem();

	QVBoxLayout *GetAboveStatesLayout();
	QVBoxLayout *GetBelowStatesLayout();

	int GetCurStateIndex();
	IStateData *GetCurStateData();
	void UpdateActions();
	void FocusState(int iStateIndex, QVariant subState);

	void ShowStates(bool bShow);
	void SetAddStateBtnEnabled(bool bEnabled);

	virtual void OnGiveMenuActions(QMenu *pMenu) = 0;
	virtual void OnUpdateActions() = 0; // TODO: Refactor IWidget::OnUpdateActions and IWidget::OnFocusState to be one function - also standardize UndoCmds to always send a minimal amount of info to widget when calling this
	virtual void OnFocusState(int iStateIndex, QVariant subState) = 0;

private Q_SLOTS:
	void OnCurrentIndexChanged(int index);

	void OnAddStateTriggered();

	void OnDuplicateStateTriggered();

	void OnRemoveStateTriggered();

	void OnRenameStateTriggered();

	void OnOrderStateBackwardsTriggered();

	void OnOrderStateForwardsTriggered();

private:
	Ui::IWidget *uiWidget;

	void AppendState(int iCopyFromState);
};

#endif // IWIDGET_H
